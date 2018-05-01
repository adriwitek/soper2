/** @file utilidades.c
 *  @brief utilidades
 *
 *  @date 17/04/2018
 *  @author Daniel
 *  @author Adrian
 *  @bug No known bugs.
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/**
* @brief genera un numero aleatorio entre dos margenes
*
* @param inf margen inferior
* @param sup margen superior
* @param id id que cambia la seed para rand
* @return el numero generado o -1 en caso de -1or
*/
int aleat_num(int inf, int sup, int id){
  int num_ale;
  if(inf > sup){
  	printf("Aleat_NUM: inferior tiene que se mas pequeño que superior\n");
  	return -1;
  }
  //srand(id);
  num_ale = ((rand()/(RAND_MAX + 1.) ) * (sup - inf + 1)) + inf;

  return num_ale;
  
}


/**
 * @brief reemplaza un caracter indicado por otro en una cadena
 *
 * @param str cadena a cambiar
 * @param find caracter a buscar
 * @param replace caracter por el que cambiar fin
 * @return cadena cambiada
 *
*/
char * replace_char(char* str, char find, char replace){

    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}


/***************************************************/
/* Funcion: medio    Fecha:    03/11/2017          */
/*                                                 */
/*                                                 */
/* Funcion que elije el pivote para la tabla       */
/* en este caso elejimos el primero                */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: indice del primer elemento              */
/* int iu: indice del segundo elemento             */
/* int *pos: indice del pivote                     */
/* Salida:                                         */
/* retorna 0                                       */
/*                                                 */
/*                                                 */
/* -1 en caso de -1or                            */
/***************************************************/

int medio(int *tabla, int ip, int iu, int *pos){
    
    if(tabla == NULL || ip<0 || ip>iu){
        return -1;
    }
    
    (*pos) = ip;
    
    return 0;
    
}

/***************************************************/
/* Funcion: Partir   Fecha:    03/11/2017          */
/*                                                 */
/*                                                 */
/* Funcion que a partir de un pivote               */
/*     coloca los elementos menores a su izq. en   */
/*      la tabla,y a su derecha los mayoeres       */
/*      "ordena" parcialmente la tabla             */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: indice del primer elemento              */
/* int iu: indice del segundo elemento             */
/* int* pos: indice del pivote                     */
/* Salida:                                         */
/* int ob: numer de veces que se ha ejecutado la   */
/* operacion basica                                */
/*                                                 */
/*                                                 */
/* -1 en caso de -1or                            */
/***************************************************/

int partir( int *tabla, int ip, int iu, int *pos){
    
    int k, aux, m, i, ob = 0, return_value = 0;
    
    if(tabla == NULL || ip<0 || ip>iu){
        return -1;
    }
    
    return_value = medio(tabla, ip, iu, pos);
    if(return_value == -1){
        return -1;
    }
    
    m = (*pos);
    
    k = tabla[m];
    /*swap*/
    aux = tabla[ip];
    tabla[ip] = tabla[m];
    tabla[m] = aux;
    /**/
    m = ip;
    
    for(i=ip+1; i<=iu; i++){
        ob ++;
        if(tabla[i]<k){
            m++;
            /*swap*//*entre posicion i y m*/
            aux = tabla[i];
            tabla[i] = tabla[m];
            tabla[m] = aux;
        }
    }
    
     /*swap*/
    aux = tabla[ip];
    tabla[ip] = tabla[m];
    tabla[m] = aux;
    (*pos) = m;
    
    return ob;
}




/***************************************************/
/* Funcion: QuickSort    Fecha:    03/11/2017      */
/*                                                 */
/*                                                 */
/* Funcion que ordena una tabla de numeros         */
/* que toma como entrada la tabla y el indice del  */
/* principio y fin deseados                        */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: indice del primer elemento              */
/* int iu: indice del segundo elemento             */
/* Salida:                                         */
/* int ob: numer de veces que se ha ejecutado la   */
/* operacion basica                                */
/*                                                 */
/*                                                 */
/* -1 en caso de -1or                            */
/***************************************************/


int quicksort(int *tabla, int ip, int iu){
    
    int ob = 0, return_value = 0;
    int pos;
    
    if(tabla == NULL || ip<0 || ip>iu){
        return -1;
    }
 
    if(ip == iu){
        return ob;
    }
    else{
        
        return_value = partir(tabla, ip, iu, &pos);
        if(return_value == -1){
            return -1;
        }
        ob += return_value;
        /*Parte izq del pivote*/
        if(ip<(pos-1)){
            return_value = quicksort(tabla, ip, pos-1);
            if(return_value == -1){
                return -1;
             }
             ob += return_value;
        }
        /*Parte derecha del pivote*/
        if((pos+1)< iu){
            return_value = quicksort(tabla, pos+1, iu);
            if(return_value == -1){
                return -1;
             }
             ob += return_value;
        }
    }
    return ob;
}