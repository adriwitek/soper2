#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "semaforos.h"
#include  <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h> /* shm* */
#include <wait.h>

#include <unistd.h>



struct info{
   int shmid;
   int *  semaforo;
};

char * crea_o_asocia_shm(int key,int * semaforo);
void destruye_shm(char * informacion);