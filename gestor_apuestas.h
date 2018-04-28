#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <string.h>


#define KEY 139  /*!<Random key value*/

typedef struct _apuesta {
  int ventanilla_id;
	int caballo_id;
	double cotizacion_caballo;
	double cantidad_apostada;
	double posible_benficio;
} apuesta;


typedef struct _apostador{
	int id; //Quien lo ha postado
	char *nombre[15];
	double total_apostado;
	int n_apuestas_realizadas;
	apuesta * apuestas_realizadas[30]; // 30 seg por apuesta,cada segundo 1,MAX 30 apuestas
	double beneficios_obtenidos;
	double dinero_ganado;
} apostador;

typedef struct _gestor_apuestas{
  int n_ventanillas;
   pthread_t * ventanillas;
  int ga_msqid;
  int * sem_ventanillas;//Zonas criticas de memoria,semaforo
  short carrera_comenzada;/*Boolean*/
  
  int n_apostadores;
  apostador * apostadores; //tamaño de 100
  int total_apostado;
  
  int n_apuestas;
  apuesta * apuestas_realizadas; //Total donde se almacen todas las apuestas tamaño de 3000 = MAX APOSTADORES * 30 seg
}gestor_apuestas;


typedef struct _Mensaje_Ventanilla{ /*!< estructura mensaje*/
	long id;  /*!< id*/ /*Tipo de mensaje*/
	char nombre_apostador[3000];  /*!< nombre del apostador*/
  int id_caballo; 
  double dinero_apuesta;
}mensaje_ventanilla; /*!< mensaje*/


---------------------------------------------
 
int crear_ventanillas(struct _gestor_apuestas * g_apuestas,caballos* e_cab, int n_ventanillas,int n_apostadores);
int ventanillas_abre_ventas(struct _gestor_apuestas * g_apuestas);
int ventanillas_cierra_ventas(struct _gestor_apuestas * g_apuestas);
void ventanilla_atiende_clientes(void *argv);
void inicializa_apuestas(struct _gestor_apuestas * g_apuestas,caballos* e_cab);
void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab);
int* get_top_10_apostadores(struct _gestor_apuestas * g_apuestas);
void liberar_gestor_apuestas(struct _gestor_apuestas * g_apuestas);
