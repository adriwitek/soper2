#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "semaforos.h"
#include  <sys/ipc.h>


char * crea_o_asocia_shm(int key,int * semaforo);
void destruye_shm(char * informacion);