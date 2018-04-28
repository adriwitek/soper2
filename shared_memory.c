	
char * crea_o_asocia_shm(int key,int * semaforo){
		
		unsigned short array_comun[2] = {1, 1}; /*1 semaforo,inicializados a 1*/
		struct info * informacion;
		int id;
		
		semaforo = (int*) malloc(sizeof (int)); 
    if (semaforo == NULL) {
        printf("Linea %d - Error al reservar memoria\n", __LINE__);
    }
    
    if (-1 == Crear_Semaforo(IPC_PRIVATE, 1, semaforo)) {/*1 semaforo*/
        printf("Linea %d - Error al crear el semaforo\n", __LINE__);
        return -1;
    }
    if (Inicializar_Semaforo(*semaforo, array_comun) == -1) {
        printf("\n Linea %d - Error al inicializar el semaforo\n", __LINE__);
        return -1;
    } else {/**/
        printf("Semaforo inicializado correctamente\n");
    }

  
  

    if((id=shmget(key,sizeof(struct info),IPC_CREAT|IPC_EXCL|0660))==-1){
			printf("El segmento de memoria compartida ya existe\n");
			return -1;
		} else {/**/
			printf("Nuevo segmento creado\n");
	}
	
		informacion = shmat (id, (char *)0, 0);
			if (informacion == NULL) {
				fprintf (stderr, "Error reserve shared memory \n");
				return -1; 
			}
		informacion->shmid = id;
		informacion->semaforo = semaforo ;
		return informacion;
	}
	
	
void destruye_shm(char * informacion){
		int id;
		Borrar_Semaforo(*( (struct info *)informacion)->semaforo)  );
		id = ( (struct info *)informacion)->shmid;
		shmdt ((char *)informacion);
	  shmctl (id, IPC_RMID, (struct shmid_ds *)NULL);
	}
	