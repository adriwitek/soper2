#include "shared_memory.h"

void main(int argc,char** argv){
    
    informacion * info;
    int * sem;
    int i;
    
    
    info = (informacion *) crea_o_asocia_shm(14856,sem,10);
    if(info == NULL){
        printf("\n Tester :fallo con la memoria");
    }
    
    
    
    /*
     info = (informacion *) crea_o_asocia_shm(123456,sem,10);
    if(info == NULL){
        printf("\n Tester :fallo con la memoria");
    }
    */
    printf("\n Todo guay por aqui");
    
    for(i=0;i<5;i++){
         info->contenido[i] = i;
    }
    
        for(i=0;i<5;i++){
         printf("\n Valor : %d",info->contenido[i]);
    }
   
   Up_Semaforo( *info->semaforo,0, SEM_UNDO);/*Lo ponemos a 1*/
   printf("\nBajamos el semaforo binario 5 segundos");
   Down_Semaforo( *info->semaforo,0, SEM_UNDO);
   sleep(5);
   printf("\nSubimos");
   Up_Semaforo( *info->semaforo,0, SEM_UNDO);
     for(i=5;i<10;i++){
         info->contenido[i] = i;
    }
    
        for(i=5;i<10;i++){
         printf("\n Valor : %d",info->contenido[i]);
    }
   
    destruye_shm((char *)info );
    printf("\n Fin del tester");
}