FLAG = -std=gnu99 -Wall -g	

all: main test 

main: main.c imprimir.o semaforos.o utilidades.o caballo.o
	gcc -o main main.c caballo.o imprimir.o semaforos.o utilidades.o

imprimir.o: 
	gcc -c imprimir.c imprimir.h

semaforos.o: 
	gcc -c semaforos.c semaforos.h $(FLAG)

caballo.o:
	gcc -c caballo.c caballo.h $(FLAG)


gestor_apuestas.o: gestor_apuestas.c gestor_apuestas.h
	gcc -c gestor_apuestas.c gestor_apuestas.h $(FLAG)
	
shared_memory.o: shared_memory.h shared_memory.c
	gcc -c shared_memory.c shared_memory.h $(FLAG)

utilidades.o: 
	gcc -c utilidades.c utilidades.h $(FLAG)	


test: test.c utilidades.o caballo.o
	gcc -o test test.c utilidades.o caballo.o





sh_tester: sh_tester.c shared_memory.o semaforos.o
	gcc -o sh_tester shared_memory.c sh_tester.c semaforos.o



main_tester: all
	./main 10 20 15 8 10000
clean:
	rm -f main test *h.gch *.o