#include "caballo.h"
#include <stdio.h>
#include <stdlib.h>

void inicializar_caballos(caballos caballos_creados){

	int i = 0;
	for(i=0;i<caballos_creados.num_caballos;i++){
		set_caballos_posicion(&caballos_creados,i,0);
		set_caballos_apostado(&caballos_creados,i,1.0);
		set_caballos_cotizacion(&caballos_creados,i,1.0);
		set_caballos_tirada(&caballos_creados,i,0);
		set_caballos_acumulado_tirada(&caballos_creados,i,0);
	}

}


/*FUNCIONES DE CABALLO*/
int get_caballo_id(caballo caballo_in){return caballo_in.id;}
int get_caballo_proceso_id(caballo caballo_in){return caballo_in.proceso_id;}
int get_caballo_posicion(caballo caballo_in){return caballo_in.posicion;}
int get_caballo_tirada(caballo caballo_in){return caballo_in.tirada;}
int get_caballo_acumulado_tirada(caballo caballo_in){return caballo_in.acumulado_tirada;}
double get_caballo_cotizacion(caballo caballo_in){return caballo_in.cotizacion;}
double get_caballo_apostado(caballo caballo_in){return caballo_in.apostado;}

void set_caballo_id(caballo *caballo_in, int i){caballo_in->id = i;}
void set_caballo_proceso_id(caballo *caballo_in, int i){caballo_in->proceso_id = i;}
void set_caballo_posicion(caballo *caballo_in, int i){caballo_in->posicion = i;}
void set_caballo_tirada(caballo *caballo_in, int i){caballo_in->tirada = i;}
void set_caballo_acumulado_tirada(caballo *caballo_in, int i){caballo_in->acumulado_tirada += i;}
void set_caballo_cotizacion(caballo *caballo_in, double i){caballo_in->cotizacion = i;}
void set_caballo_apostado(caballo *caballo_in, double i){caballo_in->apostado = i;}
/*FIN FUNCIONES DE CABALLO*/

/*FUNCIONES DE CABALLOS - LA ESTRUCTURA*/
int get_caballos_id(caballos caballos_creados, int id){return get_caballo_id(caballos_creados.caballos_registrados[id]);}
int get_caballos_proceso_id(caballos caballos_creados, int id){return get_caballo_proceso_id(caballos_creados.caballos_registrados[id]);}
int get_caballos_posicion(caballos caballos_creados, int id){return get_caballo_posicion(caballos_creados.caballos_registrados[id]);}
int get_caballos_tirada(caballos caballos_creados, int id){return get_caballo_tirada(caballos_creados.caballos_registrados[id]);}
int get_caballos_acumulado_tirada(caballos caballos_creados, int id){return get_caballo_acumulado_tirada(caballos_creados.caballos_registrados[id]);}
double get_caballos_cotizacion(caballos caballos_creados, int id){return get_caballo_cotizacion(caballos_creados.caballos_registrados[id]);}
double get_caballos_apostado(caballos caballos_creados, int id){return get_caballo_apostado(caballos_creados.caballos_registrados[id]);}
int get_caballos_total(caballos caballos_creados){return caballos_creados.total;}
int get_caballos_num_caballos(caballos caballos_creados){return caballos_creados.num_caballos;}

void set_caballos_id(caballos * caballos_creados, int id, int in){set_caballo_id(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_proceso_id(caballos * caballos_creados, int id, int in){set_caballo_proceso_id(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_posicion(caballos * caballos_creados, int id, int in){set_caballo_posicion(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_tirada(caballos * caballos_creados, int id, int in){set_caballo_tirada(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_acumulado_tirada(caballos * caballos_creados, int id, int in){set_caballo_acumulado_tirada(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_cotizacion(caballos * caballos_creados, int id, double in){set_caballo_cotizacion(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_apostado(caballos * caballos_creados, int id, double in){set_caballo_apostado(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_total(caballos * caballos_creados, int in){caballos_creados->total=in;};
void set_caballos_num_caballos(caballos * caballos_creados, int in){caballos_creados->num_caballos=in;};
/*FIN FUNCIONES DE CABALLOS*/
