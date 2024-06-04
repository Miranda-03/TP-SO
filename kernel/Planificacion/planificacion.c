#include "planificacion.h"

void inicializar_planificador(void){
    /*thread_mutex_init(&nextPidMutex,NULL);
	

	sem_init(&nuevosPcbsReady, 0, 0);
	sem_init(&grado_multiprogramacion, 0, valorMultiprogramacion);
	sem_init(&io,0,0);
	nextPid = 1;

	e_new = crear_estado(NEW);
	e_ready = crear_estado(READY);
	e_execute = crear_estado(EXECUTE);
	e_exit = crear_estado(EXIT);
	e_blocked = crear_estado(BLOCKED); */


	pthread_t  th_execute_pcb;
	pthread_create(&th_execute_pcb, NULL, (void*) ejecutarProceso, NULL);
	pthread_detach(th_execute_pcb);

	pthread_t  th_largo_plazo;
	pthread_create(&th_largo_plazo, NULL, (void*) planificadorLargoPlazo NULL);
	pthread_detach(th_largo_plazo);

	pthread_t  freeProcesoExit;
	pthread_create(&freeProcesoExit, NULL, (void*) liberarProceso, NULL);
	pthread_detach(freeProcesoExit);
}

void ejecutarProceso(void){
	while(1){
	//sem_wait(semExec);
    }   
}

void planificadorLargoPlazo(void){
	while(1){
	//sem_wait(semPlanLp);
    }
	
}

void liberarProceso(void){
	while(1){
	//sem_wait(semFree);
    }
	
}

