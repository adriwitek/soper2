#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <wait.h>

/* Para usar fstat */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "imprimir.h"
#include "semaforos.h"
#include "utilidades.h"
#include "caballo.h"
#include "shared_memory.h"
#include "gestor_apuestas.h"


#define MAX_APOSTADORES 100
#define MAX_BUFF 2048
#define KEY 123

#define SH_KEY_GESTOR	15893	/*Random values*/
#define SH_KEY_MONITOR	45782

#define TAM_SH	200				/*Tamanio en bytes de la memoria cmpartida creada*/


typedef struct _Mensaje{ /*!< estructura mensaje*/
	long id;  /*!< id*/
	char contenido[3000];  /*!< contenido mensaje*/
}mensaje; /*!< mensaje*/

typedef struct _Pid{ /*!< estructura mensaje*/
	int num;  /*!< id*/
	int pids[100];
}pid_system; /*!< mensaje*/



void trabajo_proceso_apostador(int imprimir_bonito);
void imprimir_cuenta_atras(int imprimir_bonito);




void imprimir_caballos(caballos caballos_creados);

void imprimir_podio_caballos(caballos caballos_creados, int imprimir_bonito);

void imprimir_apostadores_fin(caballos caballos_creados);

void notificar_posicion_caballos_hp(int tuberias_hijo_padre[MAX_CABALLOS][2], caballos * caballos_creados);
void notificar_posicion_caballos_fin_hp(int tuberias_hijo_padre[MAX_CABALLOS][2], caballos * caballos_creados);
void actualizar_posiciones_caballos(caballos * caballos_creados);

int determinar_tirada(int posicion_in, int num_caballos);

void manejador_SIGINT();
void manejador_SIGUSR1();

enum _e_sem_comunes {
    s_sincro_1 = 1, s_sincro_2, s_memory_pa, s_memory_pm
}e_sem_comunes;

/******************************************************************************/
struct _msg_apostador{ /*!< estructura info*/
   char nombre[20]; /*!< nombre*/
   int numero_caballo;
   double apuesta;
}msg_apostador;
/*******************************************************************************/

	int flag_y_principal = 0;
	int flag_y_monitor = 0;


int main (int argc, char ** argv){
	
	int i = 0, j = 0, pid = 0, flag = 0, i_contador = 0;
	
	int posicion_local_caballo = 0;
	int id_local_caballo = 0;
	//char * prog2[]={"nombre",NULL};

	int numero_total_procesos_sinc = 0;

	struct _caballos * caballos_creados;
	struct _sistema_apostador * sistema_apostador_creado;

	int tuberias_padre_hijo[MAX_CABALLOS][2];
	int tuberias_hijo_padre[MAX_CABALLOS][2];
	char cadena_pipes[20]="";

	sigset_t set_caballos, oset_caballos;

	int * sem_caballos;
	int * sem_inicio_caballos;
	int * sincronizacion_inicio;
	int * semaforon;
	unsigned short * array_cli = NULL;
	
/*************************************************************************************************************/
	int n_ventanillas;
	int n_apostadores;
	
	struct _gestor_apuestas * g_apuestas;
	
	informacion * sh_gestor;  /*shared memory zone*/
	informacion * sh_monitor;
	int *sem_sh_gestor;		/*Semaforos para la meoria compartida*/
	int *sem_sh_monitor;	
	
	struct _comunicacion_con_gestor comunicacion_con_gestor;
/*************************************************************************************************************/

	key_t clave_caballos; 
	int msqid_caballos; 
	mensaje msg_caballos;
	

	char cadena_aux[MAX_BUFF]="";
	char cadena_aux2[MAX_BUFF]="";
	char *cadena_pt_aux=NULL;
	char *cadena_pt_aux2=NULL;	
	int i_aux = 0, i_aux2 = 0;

	int imprimir_bonito = 0;

	int flag_fin = 0;
	/* 
	 * Forma de llamar al programa
	 *
	 * ./main nCaballos longitudCarrera nApostadores nVentanillasApostar dineroMax
	 *
	 *
	 * nCaballos -> 1 a 10
	 * nApostadores -> 1-100
	 * 
	 *
	 */

	/* 
	 *
	 *  Ejecutar en tamaño de terminal
	 *	132 x 43 
	 *
	 */

	system("clear");
	srand(getpid());
	if (argc < 6){
		printf ("Error al invocar el programa, tienes que llamarlo:\n");
		printf("\t./main nCaballos longitudCarrera nApostadores nVentanillasApostar dineroMax \n");
		printf("\t nCaballos:\tnumero de caballos que correran, de 1 a 10\n");
		printf("\t longitudCarrera:\tlongitud de carrera para los caballos\n");
		printf("\t nApostadores:\tnumero de apostadores, de 1 a 100\n");
		printf("\t nVentanillasApostar:\tnumero de ventanillas para gestionar las apuestas\n");
		printf("\t dineroMax:\tcantidad de dinero por cada apostante\n");
		return -1;
	} else {
		if(atoi(argv[1]) > MAX_CABALLOS){
			printf("Error, introduce menos de 10 caballos\n");
			return -1;
		} else if(atoi(argv[3]) > MAX_APOSTADORES) {
			printf("Error, introduce menos de 100 apostadores\n");
			return -1;
		}

		printf("Quieres activar el imprimir bonito? (0-No 1-Si)\n");
		scanf("%d",&imprimir_bonito);
		//imprimir_bonito = 0;
		if(imprimir_bonito == 1){
			printf("Imprimir bonito activado\n");
			sleep(2);//clear
			imprimir_plantilla();
			fflush(stdout);
			
		}
		
		numero_total_procesos_sinc = 0;
		numero_total_procesos_sinc = atoi(argv[1]) + 1;
		n_ventanillas = atoi(argv[4]);
		n_apostadores = atoi(argv[3]);
		
		gotoxy(2,2);
		printf("Se han introducido un total de:\n");
		printf("\t\t%d caballos para correr\n", atoi(argv[1]));
		printf("\t\t%d longitud que recorrer\n", atoi(argv[2]));
		printf("\t\t%d apostadores\n", atoi(argv[3]));
		printf("\t\t%d ventanillas para apostar\n", atoi(argv[4]));
		printf("\t\t%d dinero maximo a apostar\n", atoi(argv[5]));
	}

	sleep(2);
	
	/******************************************************************FASE DE INICIACION DE PROGRAMA (PROCESO PRINCIPAL)*********************************************/
	caballos_creados = malloc(sizeof(caballos));
	caballos_creados->caballos_registrados = malloc(sizeof(caballo)*atoi(argv[1]));
	set_caballos_num_caballos(caballos_creados,atoi(argv[1]));
	inicializar_caballos(*caballos_creados);


	//inicializacion_memoria_compartida por parte del porceso principal;
	
	
	sh_gestor = (informacion *) crea_o_asocia_shm(SH_KEY_GESTOR,sem_sh_gestor,TAM_SH);
    if(sh_gestor == NULL){
        printf("\n Fallo con la memoria compartida del proceso gestor");
    }
    
    sh_monitor = (informacion *) crea_o_asocia_shm(SH_KEY_MONITOR,sem_sh_monitor,TAM_SH);
    if(sh_monitor == NULL){
        printf("\n Fallo con la memoria compartida del proceso monitor");
    }
	
	
	comunicacion_con_gestor.caballos_iniciados = 0;
	comunicacion_con_gestor.carrera_comenzada = 0;
	comunicacion_con_gestor.caballos_creados = NULL;



	 /* Rutina por defecto */
	

	/*************************************************************************************/


    //-----------
    /****Array de semaforos 1***/
    semaforon = (int*) malloc(sizeof (int)); /* ID de la lista de los semáforos cajeros */
    if (semaforon == NULL) {
        printf("Linea %d - Error al reservar memoria\n", __LINE__);
    }
    array_cli = (unsigned short *) malloc(sizeof(e_sem_comunes) * sizeof (unsigned short));
    if (NULL == array_cli) {
        printf("Linea %d - Error al reservar memoria\n", __LINE__);
        return -1;
    }
    array_cli[0] = 4;
    /*Inicializa array a 1 ,para inicializar los semaforos de cada cajero hijo*/
    array_cli[1]=0;
    array_cli[2]=0;
    for (i = 3; i <= 4; i++) {
        array_cli[i] = 1;
    }

    if (-1 == Crear_Semaforo(IPC_PRIVATE, 3, semaforon)) {
        printf("Linea %d - Error al crear el semaforo\n", __LINE__);
        return -1;
    }
    if (Inicializar_Semaforo(*semaforon, array_cli) == -1) {
        printf("\n Linea %d - Error al inicializar semaforo\n", __LINE__);
        return -1;
    }

    free(array_cli);

	/*Cola de mensajees*/
   /*clave_caballos = ftok ("/bin/ls", KEY); 
	if (clave_caballos == (key_t) -1) { 
		perror("Error al obtener clave para cola mensajes\n"); 
		exit(EXIT_FAILURE); 
	} 

	msqid_caballos = msgget (clave_caballos, 0600 | IPC_CREAT); 
	if (msqid_caballos == -1) { 
		perror("Error al obtener identificador para cola mensajes"); 
		return(0); 
	}*/

	/***********************FIN FASE DE INICIACION***********************/
	i=0;
	do {
		if(pid=fork()){
			i++;
		} else {
			flag = 1;
		}

	} while (i < 3 && flag == 0);

	if(i==0){
		/*************************************************************************************PROCESO MONITOR***********************************************************************/

		/* ANTES DE LA CARRERA */
		if(imprimir_bonito == 1){
			gotoxy(98,3);
		}
		printf("\tSoy el proceso monitor\n");
		if(imprimir_bonito == 1){
			gotoxy(98,4);
		}
		printf("\tSegundos para empezar la carrera: \n");
		imprimir_cuenta_atras(imprimir_bonito);
		if(imprimir_bonito == 1){
			system("clear");
			gotoxy(1,1);
			imprimir_plantilla();
		}
		Up_Semaforo(*semaforon, s_sincro_1, SEM_UNDO);
		Down_Semaforo(*semaforon, s_sincro_2, SEM_UNDO);
		
		if(imprimir_bonito == 1){
			gotoxy(67,7);
		}
		
		sh_monitor = (informacion *) crea_o_asocia_shm(SH_KEY_MONITOR,sem_sh_monitor,TAM_SH);
    	if(sh_monitor == NULL){
    		 printf("\n MONITOR: Fallo con la memoria compartida ");
    	}
	
		//printf("\tEstados de las apuestas:\n");
		//sleep(5);
		/*if(imprimir_bonito == 1){
			limpiar_segundo_cuadro();
		}*/
		/****************FIN FASE ANTES DE CARRERA******************/

		/*DURANTE LA CARRERA*/

		//while(){
		//TODO

		/*

			Leer todo el rato de memoria compartida, a ver si el proceso principal le ha dicho q es el final o no
			en caso de ser el final, paramos todos los procesos

		*/
		//imprimir_segundo_cuadro_durante_carrera();
			//imprimir_caballos(caballos_creados);


		//}
		/****************FIN FASE DURANTE LA CARRERA******************/




		/*FIN CARRERA*/

		//imprimir_caballos_fin();
		//imprimir_apostadores_fin();

		/*
			Imprimir Listado de apuestas realizadas:
				- El apostador , ventanilla que gestiona la apuesta, el caballo, la cotizacion del caballo antes de la apuesta y la cantidad apostada
				(estas apuestas en orden, segun se efectuaron)
			Imprimir la posicion de los caballos, segun terminaron
			Imprimir resultado de las apuestas:
				- Nombre del apostador, cantidad apostada, beneficios obtenidos y dinero restante
		*/

		/****************FIN CARRERA******************/

		exit(EXIT_SUCCESS);
	} else if (i==1){	
		/*****************************************************************************************PROCESO GESTOR DE APUESTAS*******************************************************************************/
		struct _caballos * caballos_creados;
		int establo_creado = 0;
		int carrera_comenzada = 0;
		struct _comunicacion_con_gestor *contenido;
		
		if(imprimir_bonito == 1){
			gotoxy(2,24);
		}
		printf("\tSoy el proceso gestor de apuestas\n");


			
	     sh_gestor = (informacion *) crea_o_asocia_shm(SH_KEY_GESTOR,sem_sh_gestor,TAM_SH);
		 if(sh_gestor == NULL){
    		   printf("\n Gestor Apuestas:Fallo con la memoria compartida ");
		 }
		 contenido = (struct _comunicacion_con_gestor *)sh_gestor->contenido;
		 printf("\nHasta aqui hemos llegado2");
		 do{   /*Comprobamos que los caballos se han creado*/
		 	Down_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);/*Notificamos del inicio de la carrera al gesotr de apuestas*/
				establo_creado =contenido->caballos_iniciados;
			Up_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);
		 }while(establo_creado != 1);
		 caballos_creados = contenido->caballos_creados;
	
		 
	
		/*LEER DE LA MEMORIA COMPARTIDA SI YA HA INICIADO EL PUNTERO*/
		if(-1 ==  crear_ventanillas( g_apuestas,caballos_creados,  n_ventanillas, n_apostadores) ){
				printf("\n ERROR AL CREAR VENTANILLAS");
		}
		inicializa_apuestas(g_apuestas,caballos_creados);
		ventanillas_abre_ventas( g_apuestas);/*Lanza los hilos*/
		
		
		/*BUCLE PRINCIPAL DEL PROCESO*/
		do{
			Down_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);/*Notificamos del inicio de la carrera al gesotr de apuestas*/
				carrera_comenzada =contenido->carrera_comenzada;
			Up_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);
			/*Los hilos estan atiendo*/
		}while(carrera_comenzada != 1);
		
		/*sI YA HA COMENZADO LA CARRERA*/
		ventanillas_cierra_ventas(g_apuestas);

	

		exit(EXIT_SUCCESS);
	} else if (i==2){
		/***************************************************************************************PROCESO APOSTADOR**************************************************************************/
		//printf("\tSoy el proceso apostador\n");
		/*Igual que en los otros procesos,volver a poner cuando vaya lo otroo             */
		
		//trabajo_proceso_apostador(imprimir_bonito);
		exit(EXIT_SUCCESS);
	}
	
	
	
	
	
	
	
	/********************************************************************   PROCESO PRINCIPAL        ****************************************************************************************************************/
	
	
	/* Restringimos las señales a SOLO SIGUSR1, creado para los caballos, luego restauramos la señal
	sigfillset(&set_caballos);  
	sigdelset(&set_caballos,SIGUSR1);
	if( -1 == sigprocmask(SIG_BLOCK, &set_caballos,&oset_caballos)){ 
		printf("Linea %d - Error al hacer procmask", __LINE__);
	}

	***********************************************************************************************/
	sigemptyset(&set_caballos);  
	//sigaddset(&set_caballos,SIGUSR1);

/****************CREACION DE LOS CABALLOS******************/
	if(imprimir_bonito == 1){
		gotoxy(31,1);
	}

	for(i=0;i<atoi(argv[1]);){
		if(pipe(tuberias_padre_hijo[i])==-1) {
			printf("Linea %d - Error creando la tuberia\n", __LINE__);
			exit(EXIT_FAILURE);
		}
		if(pipe(tuberias_hijo_padre[i])==-1) {
			printf("Linea %d - Error creando la tuberia\n", __LINE__);
			exit(EXIT_FAILURE);
		}

		if(pid = fork()){
			set_caballos_id(caballos_creados,i,i);
			set_caballos_proceso_id(caballos_creados,i,pid);
			i++;
		} else {
			signal(SIGUSR1, manejador_SIGUSR1);
			id_local_caballo = i;
			//caballos_creados->caballos_registrados[i]->proceso_id = getpid();
			set_caballos_proceso_id(caballos_creados,i,getpid());
			
			if(imprimir_bonito == 1){
				fflush(stdout);
				gotoxy(70,id_local_caballo+30);
			}
			printf("\t\tCaballo %d listo %d - %d\n", i, sigismember(&set_caballos,SIGUSR1), getpid());
			//Down_Semaforo(*semaforon, s_sincro, SEM_UNDO);
			/*Con este down se hace un system clear*/
			if(imprimir_bonito == 1){
				gotoxy(70,id_local_caballo+24+flag_y_monitor);
				flag_y_monitor++;
			}
			pause();
			do{
				close(tuberias_padre_hijo[id_local_caballo][1]);
	        	read(tuberias_padre_hijo[id_local_caballo][0], cadena_pipes, sizeof(cadena_pipes));
	        
	        	posicion_local_caballo = atoi(cadena_pipes);
	        	if(posicion_local_caballo >= 0){
	        		if(imprimir_bonito == 0){
	        			printf("Caballo %d - Proceso principal me dice mi posicion: %d\n", id_local_caballo, posicion_local_caballo);
	        		} else {
	        			gotoxy(40,4+id_local_caballo);
	        			printf("Caballo%d\t%d\n", id_local_caballo, posicion_local_caballo);
	        		}
		        	i_aux = determinar_tirada(posicion_local_caballo, atoi(argv[1]));
					memset(&cadena_pipes[0],0,5);
		        	sprintf(cadena_pipes,"%d-%d",id_local_caballo,i_aux);
		        	if(imprimir_bonito == 1){
						gotoxy(70,id_local_caballo+24+flag_y_monitor);
						flag_y_monitor++;
					}
		        	close(tuberias_hijo_padre[id_local_caballo][0]);
	        		write(tuberias_hijo_padre[id_local_caballo][1], cadena_pipes, strlen(cadena_pipes));
	        		if(imprimir_bonito == 1){
						gotoxy(70,id_local_caballo+24+flag_y_monitor);
						flag_y_monitor++;
					}
					sleep(1);
	        	}
			} while (posicion_local_caballo != -1);

			printf("Soy el caballo %d y me retiro a los establos\n",id_local_caballo);
			exit(EXIT_SUCCESS);
		}
	}
	//MODIFICAR
	
	/*escribir en memoria compartida gestor que los cabllos estan ya listos y pasarle la estructura*/
	Down_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);
		sh_gestor->contenido = &comunicacion_con_gestor;
		((struct _comunicacion_con_gestor*)(sh_gestor->contenido) )->caballos_iniciados = 1;
		/*	printf("\n\nMIIIIRAAA ESTO FURRULA : %d",((struct _comunicacion_con_gestor*)(sh_gestor->contenido) )->caballos_iniciados);*/
		((struct _comunicacion_con_gestor*)(sh_gestor->contenido) )->caballos_creados = caballos_creados;
	Up_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);
	
	signal(SIGINT, manejador_SIGINT); /* Rutina por defecto */

	/* Restauramos la señal*/
	sigemptyset(&set_caballos);  
	sigaddset(&set_caballos,SIGUSR1);
	if( -1 == sigprocmask(SIG_BLOCK, &set_caballos,&oset_caballos)){ /*Bloquea la recepción de las señales */
		printf("Linea %d - Error al hacer procmask", __LINE__);
	}
	/***********************************************************************************************/

		printf("\n\n\n\nHasta aqui hemos llegado3");
	Down_Semaforo(*semaforon, s_sincro_1, SEM_UNDO);
	Up_Semaforo(*semaforon, s_sincro_2, SEM_UNDO);
	
	if(imprimir_bonito==1){
		imprimir_hipodromo();
	}

	i_contador = 0;
	


	
	
	
	

	
	
	Down_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);/*Notificamos del inicio de la carrera al gesotr de apuestas*/
		((struct _comunicacion_con_gestor*)(sh_gestor->contenido) )->carrera_comenzada = 1;
	Up_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);

																										/**********EMPIEZA LA CARRERA**************/
	if(NULL==(cadena_pt_aux = malloc(sizeof(char))))return -1;
	for(j=0;j<get_caballos_num_caballos(*caballos_creados);j++){
		if(imprimir_bonito == 1){
			gotoxy(39,flag_y_principal+30);
			flag_y_principal ++;
		}
		kill(get_caballos_proceso_id(*caballos_creados,j),SIGUSR1);																
	}
	do{
		if(imprimir_bonito == 0){
			printf("---------------------------------------------------------RONDA %d---------------------------------------------\n", i_contador);
		}
		
		notificar_posicion_caballos_hp(tuberias_padre_hijo, caballos_creados);

		for(j=0;j<get_caballos_num_caballos(*caballos_creados);){
			memset(&cadena_pipes[0],0,5);
			close(tuberias_hijo_padre[j][1]);
	        read(tuberias_hijo_padre[j][0], cadena_pipes, sizeof(cadena_pipes));	
	        /* El mensaje contiene primero el id del caballo y luego su tirada*/
			//strcpy(cadena_pipes,"1-4");
			cadena_pt_aux=strtok(cadena_pipes,"-");
			i_aux = atoi(cadena_pt_aux);
			cadena_pt_aux=strtok(NULL,"-");
			i_aux2 = atoi(cadena_pt_aux);
			set_caballos_tirada(caballos_creados,i_aux,i_aux2);
			set_caballos_acumulado_tirada(caballos_creados,i_aux,i_aux2);
			if(imprimir_bonito==0){
				printf("Caballo %d con %d\n", get_caballos_id(*caballos_creados,i_aux),get_caballos_acumulado_tirada(*caballos_creados,i_aux));
			} else {
				imprimir_podio_caballos(*caballos_creados, imprimir_bonito);			
			}
			if( get_caballos_acumulado_tirada(*caballos_creados,j) >= atoi(argv[2])){
				if(imprimir_bonito == 0){
					printf ("HAY UN GANADOR - Caballo %d\n", i_aux);
				}
				flag_fin = 1;																															/*fin de la carrera*/
			}
			j++;

		}
/*		free(cadena_pt_aux);
		cadena_pt_aux = NULL;*/

		actualizar_posiciones_caballos(caballos_creados);
		i_contador ++;
		sleep(1);
	}while(flag_fin == 0);
																																					/*carrera acabada*/
	notificar_posicion_caballos_fin_hp(tuberias_padre_hijo, caballos_creados);
	//decir por memoria compartida que ya esta terminado
	
	imprimir_podio_caballos(*caballos_creados, imprimir_bonito);


	while(1);
	while(wait(NULL)>0);	
	destruye_shm((char *)sh_gestor );
	destruye_shm((char *)sh_monitor );
	return 1;

}


void imprimir_cuenta_atras(int imprimir_bonito){
	
	int i = 0;

	for(i=5;i>=0;i--){
		if(imprimir_bonito == 1){
			gotoxy(108,5);
			printf("%d\n", i);
		} else {
			printf("Cuenta atras: %d\n", i);
		}
		sleep(1);	
	}
	
}
/****************************************************************************************************************************************/
void trabajo_proceso_apostador(int imprimir_bonito){

	int i = 0;
	for(i=0;i<=5;i++){
		sleep(1);
		if(imprimir_bonito == 1){
			gotoxy(67,25);
			printf("\tEnviando apuesta: %d\n", i);
		} else {
			printf("Enviando apuesta: %d\n", i);
		}
	}

}

/*************************************************************************************************************************************************/




void imprimir_caballos(caballos caballos_creados){
	int i = 0;
	for(i=0;i<caballos_creados.num_caballos;i++){
		printf("Caballo %d en posicion %d\n", i, get_caballos_posicion(caballos_creados,i));
		printf("Caballo %d ultima tirada %d\n", i, get_caballos_tirada(caballos_creados,i));
	}
}

void imprimir_caballos_fin(caballos caballos_creados){
	int i = 0, j=0, flag = 0;
	for(j=0;j<caballos_creados.num_caballos;j++){
		flag = 0;
		for(i=0;i<caballos_creados.num_caballos;i++){
			if(j== get_caballos_posicion(caballos_creados,i)){
				printf("Caballo %d en posicion %d\n", i, get_caballos_posicion(caballos_creados,i));
				printf("Caballo %d ultima tirada %d\n", i, get_caballos_tirada(caballos_creados,i));
				flag = 1;
			}
		}
		if(flag==0){
			printf("No se ha encontrado ningun caballo en la posicion %d\n", i);
		}

	}
}

void imprimir_podio_caballos(caballos caballos_creados, int imprimir_bonito){

	int i = 0, j = 0;
	for(i=0;i<get_caballos_num_caballos(caballos_creados);i++){
		for(j=0;j<get_caballos_num_caballos(caballos_creados);j++){
			if(get_caballos_posicion(caballos_creados,j)==i){
				if(imprimir_bonito == 1){
					gotoxy(50, 36+i);
				}
				printf("Caballo %d - En el puesto %d - Tirada %d - Acum %d\n",
				 get_caballos_id(caballos_creados,j), get_caballos_posicion(caballos_creados,j),
				 get_caballos_tirada(caballos_creados,j), get_caballos_acumulado_tirada(caballos_creados,j));
			}
		}
	}

}

void imprimir_apostadores_fin(caballos caballos_creados){
	int i = 0;
	for(i=0;i<caballos_creados.num_caballos;i++){
		printf("Caballo %d en posicion %d\n", i, get_caballos_posicion(caballos_creados,i));
		printf("Caballo %d ultima tirada %d\n", i, get_caballos_tirada(caballos_creados,i));
	}
}



void notificar_posicion_caballos_hp(int tuberias_padre_hijo[MAX_CABALLOS][2], caballos *caballos_creados){
	int i = 0;
	char cadena_pipes[5]="";

	
	for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
		strcpy(cadena_pipes,"");
		sprintf(cadena_pipes,"%d", get_caballos_posicion(*caballos_creados,i));
        close(tuberias_padre_hijo[i][0]);
        write(tuberias_padre_hijo[i][1], cadena_pipes, strlen(cadena_pipes));
	}

}


void notificar_posicion_caballos_fin_hp(int tuberias_padre_hijo[MAX_CABALLOS][2], caballos *caballos_creados){
	int i = 0;
	char cadena_pipes[5]="";

	
	for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
		strcpy(cadena_pipes,"");
		sprintf(cadena_pipes,"-1");
        close(tuberias_padre_hijo[i][0]);
        write(tuberias_padre_hijo[i][1], cadena_pipes, strlen(cadena_pipes));
	}
}

/**
*	@brief Manejador de señal SIGUSR1
*/
void manejador_SIGINT (int sig){
	
	printf("SIGINT recibido\n");
	kill(SIGINT,0);
	while(wait(NULL)>0);
}


/**
*	@brief Manejador de señal SIGUSR1
*/
void manejador_SIGUSR1(int sig){
	
	//printf("SIGUSR1 recibido, por pid %d\n", getpid());
	signal(sig, manejador_SIGUSR1); /* Restaura rutina por defecto */
}

int determinar_tirada(int posicion_in, int num_caballos){
	
	int ultimo = num_caballos-1;
	int primero = 0;
	int posicion = posicion_in;
	int retorno = 0;
	
	if(posicion == ultimo ){
		retorno = (aleat_num(1,6,posicion)+aleat_num(1,6,aleat_num(1,6,posicion)));
		printf("ULTIMO %d\n",retorno);
	} else if (posicion == primero){
		retorno = aleat_num(1,7,posicion);
		printf("PRIMERO %d\n",retorno);
	} else {
		retorno = aleat_num(1,6,posicion);
		printf("NORMAL %d\n",retorno);
	}

	return retorno;
}

void actualizar_posiciones_caballos(caballos * caballos_creados){

	int i=0;
	int *array;
	int flag_posicion = 0, contador = 0, posicion_int = 0, max = 0;

	array = malloc(sizeof(int)* get_caballos_num_caballos(*caballos_creados));
	if(array != NULL){
		posicion_int = get_caballos_num_caballos(*caballos_creados);
		for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
			*(array+i)= get_caballos_acumulado_tirada(*caballos_creados,i);
		}
		get_caballos_num_caballos(*caballos_creados);
		for(contador=0;contador<get_caballos_num_caballos(*caballos_creados);contador++){
			for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
				if(*(array+i)>max) {
					max = array[i];
					flag_posicion = i;
				}
			}
			*(array+flag_posicion) = 0;
			set_caballos_posicion(caballos_creados,flag_posicion,get_caballos_num_caballos(*caballos_creados)-posicion_int);
			posicion_int --;
			max = 0;
		}	
	} else {
		printf("Error en actualizar_posiciones_caballos\n");
	}

}
