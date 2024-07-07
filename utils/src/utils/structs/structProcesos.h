#ifndef PROCESO_STRUCT
#define PROCESO_STRUCT

#include <stdint.h>
#include "structCpu.h"
#include <utils/enums/EstadoDeEjecucion.h>
#include <pthread.h>

/**
 * @brief struct para administrar los procesos
 * @param pid  Identificador del proceso.
 * @param pc program counter.
 * @param quantum  Unidad de tiempo utilizada por el algoritmo de planificación VRR.
 * @param registros Registros  de la CPU.
 * @param SI Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string.
 * @param DI Contiene la dirección lógica de memoria de destino a donde se va a copiar un string.
 * @param estado Estado del proceso.
 */
typedef struct
{
    uint32_t pid;
    uint32_t pc;
    uint32_t quantumRestante;
    Registros registros;
    uint32_t SI;
    uint32_t DI;
    EstadoProceso estado;
} Pcb; 

#endif