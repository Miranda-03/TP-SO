#ifndef PROCESO_STRUCT
#define PROCESO_STRUCT

#include <stdint.h>
#include "structCpu.h"
#include <utils/enums/EstadoDeEjecucion.h>
#include <pthread.h>

int ID = 1;
pthread_mutex_t mutex;


/**
 * @brief struct para administrar los procesos
 * @param pid  Identificador del proceso. 
 * @param quantum  Unidad de tiempo utilizada por el algoritmo de planificación VRR.
 * @param registros Registros  de la CPU.
 */
typedef struct
{
    int pid;
    int quantum;
    Registros registros;
} Pcb;

/**
 * @brief struct para administrar los procesos
 * @param pid  Identificador del proceso. 
 * @param quantum  Unidad de tiempo utilizada por el algoritmo de planificación VRR.
 * @param RegistrosCpu Registros  de la CPU.
 */
typedef struct
{
    Pcb pcb;
    ContextoDeEjecucion contexto;
} Proceso;

/**
 * @brief struct para administrar los procesos
 * @param info puntero al siguiente de la cola
 * @param RegistrosCpu Registros  de la CPU.
 */
typedef struct
{
    Proceso info;
    ColaDeProcesos *sig;
} ColaDeProcesos;


#endif