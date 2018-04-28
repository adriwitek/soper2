
#include"gestor_apuestas.h"

struct parametros{
    struct _gestor_apuestas * g_apuestas;
    caballos* e_cab;
    int id;/*Identificador de la ventanilla*/
};


int crear_ventanillas(struct _gestor_apuestas * g_apuestas,caballos* e_cab, int n_ventanillas,int n_apostadores){
  
  int i;
  int clave;
  unsigned short array_comun[2] = {1, 0}; /*1 semaforos,inicializados a 0*/
  struct parametros p;
  
  if(g_apuestas == NULLL){
    return -1;
  }
  g_apuestas->carrera_finalizada=0;
  g_apuestas->n_ventanillas = n_ventanillas;
  g_apuestas->ventanillas = (pthread_t*)malloc(n_ventanillas*sizeof(pthread_t));
  if(g_apuestas->ventanillas == NULL){
     printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
  }
  
  p.g_apuestas = g_apuestas;
  p.e_cab = e_cab;
  for(i=0;i<n_ventanillas;i++){
     p.id = i;
    if(  pthread_create(&g_apuestas->ventanillas[i],NULL, ventanilla_atiende_clientes ,(void*)&p )  ){
       printf("Modulo Apuestas: Linea %d - Error al crear el hilo\n", __LINE__);
       free(g_apuestas->ventanillas);
      return -1;
    }
  }
  
   g_apuestas->total_apostado = 0;
   g_apuestas->n_apuestas = 0;
   g_apuestas->n_apostadores = n_apostadores;
   g_apuestas->apostadores = (apostador*)malloc(n_apostadores*sizeof(apostador));
   if(g_apuestas->apostadores == NULL){
     printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
  }
  for(i=0;i<n_apostadores;i++){/*Inicializamos los apostadores*/
    g_apuestas->apostadores[i]->id=i;
    sprintf( g_apuestas->->apostadores[i]->nombre,"Apostador-%d",i);
    g_apuestas->apostadores[i]->total_apostado=0;
    g_apuestas->apostadores[i]->n_apuestas_realizadas = 0 ;
    g_apuestas->apostadores[i]->beneficios_obtenidos = 0;
    g_apuestas->apostadores[i]->dinero_gando = 0;
  }
  
  
  /*Creamos la cola de mensajes del gestorde apuestas*/
    clave = ftok ("/bin/ls", KEY); 
	if (clave == (key_t) -1) { 
		 perror("Modulo apuestas : Error al obtener clave para cola mensajes\n"); 
         free(g_apuestas->ventanillas);
		 return -1;
	} 

	g_apuestas->ga_msqid = msgget (clave, 0600 | IPC_CREAT); 
	if (g_apuestas->ga_msqid == -1) { 
		perror("Modulo apuestas : Error al obtener identificador para cola mensajes"); 
        free(g_apuestas->ventanillas);
		  return -1; 
  }
  
  
   /*Creamos el semaforo ventanillas,para acerptar apuestas*/
    g_apuestas->sem_ventanillas = (int*) malloc(sizeof (int)); 
    if (g_apuestas->sem_ventanillas == NULL) {
        printf("Modulo apuestas : Linea %d - Error al reservar memoria\n", __LINE__);
    }
   

    if (-1 == Crear_Semaforo(IPC_PRIVATE, 1, g_apuestas->sem_ventanillas)) {
        printf("Modulo apuestas :Linea %d - Error al crear el semaforo\n", __LINE__);
        return -1;
    }
    if (Inicializar_Semaforo(*g_apuestas->sem_ventanillas, array_comun) == -1) {
        printf("\n Modulo apuestas :Linea %d - Error al inicializar el semaforo\n", __LINE__);
        return -1;
    } else {
        printf("Modulo apuestas :Semaforo inicializado correctamente\n");
    }
  
  return 1; /*Ok*/
}



int ventanillas_abre_ventas(struct _gestor_apuestas * g_apuestas){
  int i;
  if(g_apuestas ==NULL){
    return -1;  
  }
  
  for(i=0;i<g_apuestas->n_ventanillas;i++){
    pthread_join(ventanillas[i],NULL);
  }
  Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	/*Ponemos a 1 el semaforo*/
  return 1;
}


int ventanillas_cierra_ventas(struct _gestor_apuestas * g_apuestas){
    if(g_apuestas ==NULL){
    return -1;  
  }
   Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
        g_apuestas->carrera_comenzada=1;
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
}




void ventanilla_atiende_clientes(void *argv){
   if(g_apuestas == NULLL){
    return -1;
  }
  struct parametros *q;
  q = (struct parametros*)argv;
  struct _gestor_apuestas * g_apuestas  =  q->g_apuestas;
  caballos* e_cab = q->e_cab;
  int id = q->id;/*Identificador de la ventanilla*/
  apuesta* apuesta;
  mensaje_ventanilla msg;
  int id_apostador,i;
  
  
  while(ga->carrera_comenzadaa != 1){/*Atiende mientras no halla comenzado*/
   
    msgrcv (ga->ga_msqid, (struct msgbuf *) &msg, sizeof(mensaje_ventanilla) - sizeof(long),   1, 0); 
    id_apostador = atoi(strtok(msg.nombre_apostador,"Apostador-"));
    Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
          g_apuestas->total_apostado += msg.dineroapuesta;
          g_apuestas->apostadores[id_apostador]->total_apostado +=   msg.dineroapuesta
          apuesta = (apuesta*)malloc(sizeof(apuesta));
          if(apuesta == NULL){
               printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
              return;
          }
          apuesta->caballo_id= msg.id_caballo;
          apuesta->cantidad_apostada = msg.dineroapuesta;
          apuesta->cotizacion_caballo = get_caballos_cotizacion(e_cab, get_caballos_id(e_cab, msg.id_caballo));
          apuesta->posible_benficio = msg.dineroapuesta * apuesta->cotizacion_caballo;
          apuesta->ventanilla = id;
          
          
          g_apuestas->apuestas_realizadas[ g_apuestas->apuestas] = apuesta; /*Anniadimos a la lista general de apuestas*/
          g_apuestas->apuestas++;
          
          for(i=0;i<g_apuestas->n_apostadores;i++){/*Anniadimos a las apuestas del apostador*/
              if(g_apuestas->apostadores[i]->id == id_apostador){
                  g_apuestas->apostadores[i]->apuestas_realizadas[g_apuestas->apostadores[i]->n_apuestas_realizadas] = apuesta;
                  g_apuestas->apostadores[i]->n_apuestas_realizadas++;
              }
          }
          actualizar_cotizaciones_caballos( g_apuestas,e_cab);
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
  } 
  
  pthread_exit(NULL);
}




void inicializa_apuestas(struct _gestor_apuestas * g_apuestas,caballos* e_cab){
	if(e_cab == NULL){
		return;
	}
	int i;

	
	for(i=0;i<  get_caballos_total(e_cab);i++){/*Inicializar apuestas ccabllos*/
		set_caballos_apostado(e_cab, get_caballos_id(e_cab, i), 1.0);
		set_caballos_cotizacion(e_cab,get_caballos_id(e_cab, i),0.0);
	}
	
	for(i=0;i<g_apuestas->n_apostadores  ;i++){/*Inicializar apostadores*/
	    g_apuestas->apostadores[i]->total_apostado = 0.0;
	}
	return;
}


void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab){
	if(e_cab == NULL){
		return;
	}
	
	for(i=0;i<  get_caballos_total(e_cab);i++){
		set_caballos_apostado(e_cab, get_caballos_id(e_cab, i), 1.0);
		set_caballos_cotizacion(e_cab,get_caballos_id(e_cab, i), get_total_apostado(g_apuestas)  / get_caballos_apostado(e_cab,  get_caballos_id(e_cab, i))   );
	}
	return;
}




int* get_top_10_apostadores(struct _gestor_apuestas * g_apuestas){}

double get_total_apostado(struct _gestor_apuestas * g_apuestas){
    double apostado;
    if(g_apuestas == NULLL){
    return -1;
  }
    Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
     apostado = g_apuestas->total_apostado;
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
  return apostado;
}

void liberar_gestor_apuestas(struct _gestor_apuestas * g_apuestas){
  if(g_apuestas == NULLL){
    return -1;
  }
   Borrar_Semaforo(*g_apuestas->sem_ventanillas);
   msgctl (g_apuestas->ga_msqid, IPC_RMID, (struct msqid_ds *)NULL);/*Borramos cola de mensajes*/
   free(g_apuestas->ventanillas);
 ///////////////////**************BUCLE APUESTAS DE CADA APOSTADOR********/ 
   free(g_apuestas->apostadores);
   free(g_apuestas->apuestas_realizadas);
}
