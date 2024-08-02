#ifndef PLANIFICADOR_LP_H
#define PLANIFICADOR_LP_H

#include <utils/structs/structProcesos.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <Planificadores/planificadorCP.h>

#define PATH_CONFIG "kernel.config"



typedef struct
{
    Pcb *proceso;
    char *path_instrucciones;
} PcbGuardarEnNEW;

typedef struct
{
    sem_t sem_multiprogramacion;
    int valor_inicial;
    pthread_mutex_t mutex;
    unsigned int procesos_ejecutando;  
} StructMultiprogramacion;



void init_sem_multiprogramacion(StructMultiprogramacion *msem, int value);

void post_multiprogramacion();

void wait_multiprogramacion();

/**
* @fn    inicarPlanificadorLargoPLazo
* @brief iniciar datos del planificador a largo plazo.
*/
void inicarPlanificadorLargoPLazo(int socketMemoria, char *path_config);

/**
* @fn    nuevoProceso
* @brief crea el proceso y lo guarda en la cola NEW.
*/
void nuevoProceso(char *path_instrucciones);

/**
* @fn    agregarProcesoColaNew
* @brief agrega al proceso a NEW.
*/
void agregarProcesoColaNew(PcbGuardarEnNEW *proceso);

/**
* @fn    PLPNuevoProceso
* @brief funcion para crear un nuevo proceso.
*/
void PLPNuevoProceso(char *path_instrucciones);

/**
* @fn    agregarNuevoProcesoReady
* @brief envia al proceso a Ready decrementado el GM.
*/
void *agregarNuevoProcesoReady(void *ptr);

/**
* @fn    guardarInstruccionesMemoria
* @brief guarda las instrucciones en memoria.
*/
int guardarInstruccionesMemoria(PcbGuardarEnNEW *proceso);

/**
* @fn    esperarRespuesteDeMemoria
* @brief espera la respuesta de memoria.
*/
int esperarRespuesteDeMemoria();

/**
* @fn    sacarProcesoDeNew
* @brief quita el proceso de la cola NEW y lo retorna.
*/
PcbGuardarEnNEW *sacarProcesoDeNew();

/**
* @fn    crearProcesoEstadoNEW
* @brief crea el PCB en estado NEW.
*/
Pcb *crearProcesoEstadoNEW();

/**
* @fn    asignar_pid
* @brief asigna un PID al PCB.
*/
int asignar_pid();

/**
* @fn    iniciarMutex
* @brief inicializa los mutex.
*/
void iniciarMutex();

/**
* @fn    terminarProceso
* @brief termina el proceso.
*/
void terminarProceso(Pcb *proceso, char *motivo_exit);

/**
* @fn    quitarMemoria
* @brief libera la memoria del proceso.
*/
void quitarMemoria(Pcb *proceso);

/**
* @fn    enviarMensajeMemoria
* @brief envia mensaje a la memoria con un mutex.
*/
void enviarMensajeMemoria(t_buffer *buffer, int *resultado);

/**
* @fn    ajustar_grado_multiprogramacion
* @brief ajustar el grado de multiprogramacion del planificador de largo plazo.
*/
void ajustar_grado_multiprogramacion(int nuevo_valor);
/**
* @fn    encontrar_en_new_y_terminar
* @brief busca el proceso en la cola de NEW y lo manda a la cola de exit.
*/
int encontrar_en_new_y_terminar(int pid);

void mensaje_nuevo_proceso(int pid);

void mensaje_exit(int pid, char *motivo_exit);

void listar_estados_lp();

#endif
