
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
  struct parametros p; /*Para las ventanillas(hilos)*/
  apostador *ap;
  if(g_apuestas == NULL){
    return -1;
  }
  g_apuestas->carrera_comenzada=0;
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
    ap = get_apostador_by_id( g_apuestas, i);
    ap->id=i;
    sprintf( ap->nombre,"Apostador-%d",i);
    ap->total_apostado=0;
    ap->n_apuestas_realizadas = 0 ;
    ap->beneficios_obtenidos = 0;
    ap->dinero_ganado = 0;
  }
  
  
  /*Creamos la cola de mensajes del gestorde apuestas*/
    clave = ftok ("/bin/ls", KEY_G); 
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
    pthread_join(g_apuestas->ventanillas[i],NULL);
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
    return 1;
}




void * ventanilla_atiende_clientes(void *argv){ /*Para los hilos solo*/
  
  struct parametros *q;
  apostador *ap;
  q = (struct parametros*)argv;
  struct _gestor_apuestas * g_apuestas  =  q->g_apuestas;
   if(g_apuestas == NULL){
    exit(EXIT_FAILURE) ;
  }
  caballos* e_cab = q->e_cab;
  int id = q->id;/*Identificador de la ventanilla*/
  apuesta * apuesta;
  mensaje_ventanilla msg;
  int id_apostador;
  
  
  while( g_apuestas->carrera_comenzada != 1){/*Atiende mientras no halla comenzado*/
   
    msgrcv ( g_apuestas->ga_msqid, (struct msgbuf *) &msg, sizeof(mensaje_ventanilla) - sizeof(long),   1, 0); 
    id_apostador = atoi(strtok(msg.nombre_apostador,"Apostador-"));
    ap =  get_apostador_by_id(g_apuestas, id_apostador);
    Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
          g_apuestas->total_apostado += msg.dinero_apuesta;
          ap->total_apostado +=   msg.dinero_apuesta;
        
          apuesta = (struct _apuesta*)malloc( 1* sizeof(apuesta)  );
          if(apuesta == NULL){
               printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
               exit(EXIT_FAILURE);
          }
          apuesta->caballo_id= msg.id_caballo;
          apuesta->cantidad_apostada = msg.dinero_apuesta;
          apuesta->cotizacion_caballo = get_caballos_cotizacion(*e_cab, get_caballos_id(*e_cab, msg.id_caballo));
          apuesta->posible_benficio = msg.dinero_apuesta * apuesta->cotizacion_caballo;
          apuesta->ventanilla_id = id;
          
          
          g_apuestas->apuestas_realizadas[ g_apuestas->n_apuestas] = apuesta; /*Anniadimos a la lista general de apuestas*/
          g_apuestas->n_apuestas++;
          
          
          ap->apuestas_realizadas[ap->n_apuestas_realizadas] = apuesta;
          ap->n_apuestas_realizadas++;
             
          actualizar_cotizaciones_caballos( g_apuestas,e_cab);
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
  } 
  
  pthread_exit(NULL);
}




void inicializa_apuestas(struct _gestor_apuestas * g_apuestas,caballos * e_cab){
	if(e_cab == NULL){
		return;
	}
	int i;
    apostador * ap;
	
	for(i=0;i<  get_caballos_total(*e_cab);i++){/*Inicializar apuestas ccabllos*/
		set_caballos_apostado(e_cab, get_caballos_id(*e_cab, i), 1.0);
		set_caballos_cotizacion(e_cab,get_caballos_id(*e_cab, i),0.0);
	}
	
	for(i=0;i<g_apuestas->n_apostadores  ;i++){/*Inicializar apostadores*/
    	ap = get_apostador_by_id( g_apuestas,i);
	    ap->total_apostado = 0.0;
	}
	return;
}


void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab){/*Llamada desde los hilos*/
	if(e_cab == NULL){
		return;
	}
	int i =0;
	
	for(i=0;i<  get_caballos_total(*e_cab);i++){
		set_caballos_apostado(e_cab, get_caballos_id(*e_cab, i), 1.0);
		set_caballos_cotizacion(e_cab,get_caballos_id(*e_cab, i), get_total_apostado(g_apuestas)  / get_caballos_apostado(*e_cab,  get_caballos_id(*e_cab, i))   );
	}
	return;
}




/*int* get_top_10_apostadores(struct _gestor_apuestas * g_apuestas){}*/

double get_total_apostado(struct _gestor_apuestas * g_apuestas){
    double apostado;
    if(g_apuestas == NULL){
    return -1;
  }
    Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
     apostado = g_apuestas->total_apostado;
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
  return apostado;
}

void liberar_gestor_apuestas(struct _gestor_apuestas * g_apuestas){
  if(g_apuestas == NULL){
    return;
  }
   Borrar_Semaforo(*g_apuestas->sem_ventanillas);
   msgctl (g_apuestas->ga_msqid, IPC_RMID, (struct msqid_ds *)NULL);/*Borramos cola de mensajes*/
   free(g_apuestas->ventanillas);
 ///////////////////**************BUCLE APUESTAS DE CADA APOSTADOR********/ 
   free(g_apuestas->apostadores);
   free(g_apuestas->apuestas_realizadas);
   return;
}

apostador * get_apostador_by_id(struct _gestor_apuestas * g_apuestas,int id){
    if(!g_apuestas){
        return NULL;
    }
    return  &g_apuestas->apostadores[id];
}