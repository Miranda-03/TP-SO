#ifndef PROCESO_STRUCT
#define PROCESO_STRUCT

#include <stdint.h>
#include "structCpu.h"
#include <utils/enums/EstadoDeEjecucion.h>
#include <pthread.h>





/**
 * @brief struct para administrar los procesos
 * @param pid  Identificador del proceso. 
 * @param quantum  Unidad de tiempo utilizada por el algoritmo de planificación VRR.
 * @param registros Registros  de la CPU.
 */
typedef struct
{
    uint32_t pid;
    uint32_t quantum;
    Registros registros;
    ContextoDeEjecucion contexto;
} Pcb;





#endif