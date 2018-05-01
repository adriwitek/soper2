#ifndef caballo_h
#define caballo_h





#define MAX_CABALLOS 10

typedef struct _caballo{
	int id;
	int proceso_id;
	int posicion;
	int tirada;
	int acumulado_tirada;
	float cotizacion;
	float apostado;
} caballo;

typedef struct _caballos{
	int total;
	int num_caballos;
	caballo * caballos_registrados;
} caballos;

void inicializar_caballos(caballos caballos_creados);


/*FUNCIONES CABALLO*/
int get_caballo_id(caballo caballo_in);
int get_caballo_proceso_id(caballo caballo_in);
int get_caballo_posicion(caballo caballo_in);
int get_caballo_tirada(caballo caballo_in);
int get_caballo_acumulado_tirada(caballo caballo_in);
double get_caballo_cotizacion(caballo caballo_in);
double get_caballo_apostado(caballo caballo_in);

void set_caballo_id(caballo * caballo_in, int i);
void set_caballo_proceso_id(caballo * caballo_in, int i);
void set_caballo_posicion(caballo * caballo_in, int i);
void set_caballo_tirada(caballo * caballo_in, int i);
void set_caballo_acumulado_tirada(caballo * caballo_in, int i);
void set_caballo_cotizacion(caballo * caballo_in, double i);
void set_caballo_apostado(caballo * caballo_in, double i);
/*FIN FUNCIONES CABALLO*/


/*FUNCIONES DE CABALLOS - LA ESTRUCTURA*/
int get_caballos_id(caballos caballos_creados, int id);
int get_caballos_proceso_id(caballos caballos_creados, int id);
int get_caballos_posicion(caballos caballos_creados, int id);
int get_caballos_tirada(caballos caballos_creados, int id);
int get_caballos_acumulado_tirada(caballos caballos_creados, int id);
double get_caballos_cotizacion(caballos caballos_creados, int id);
double get_caballos_apostado(caballos caballos_creados, int id);
int get_caballos_total(caballos caballos_creados);
int get_caballos_num_caballos(caballos caballos_creados);

void set_caballos_id(caballos * caballos_creados, int id, int in);
void set_caballos_proceso_id(caballos * caballos_creados, int id, int in);
void set_caballos_posicion(caballos * caballos_creados, int id, int in);
void set_caballos_tirada(caballos * caballos_creados, int id, int in);
void set_caballos_acumulado_tirada(caballos * caballos_creados, int id, int in);
void set_caballos_cotizacion(caballos * caballos_creados, int id, double in);
void set_caballos_apostado(caballos * caballos_creados, int id, double in);
void set_caballos_total(caballos * caballos_creados, int in);
void set_caballos_num_caballos(caballos * caballos_creados, int in);
/*FIN FUNCIONES DE CABALLOS*/
#endif 