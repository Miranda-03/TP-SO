#ifndef PROCESO_STRUCT
#define PROCESO_STRUCT

#include <stdint.h>
#include "structCpu.h"
#include <utils/enums/EstadoDeEjecucion.h>


/**
 * @brief struct de registros que se mandan a la cpu
 */

typedef struct
{
    uint32_t pc;
    uint8_t ax;
    uint32_t eax;
    uint8_t bx;
    uint32_t ebx;
    uint8_t cx;
    uint32_t ecx;
    uint8_t dx;
    uint32_t edx;
} Registros;

/**
 * @brief struct para administrar los procesos
 * @param pid  Identificador del proceso. 
 * @param quantum  Unidad de tiempo utilizada por el algoritmo de planificación RR y VRR.
 * @param duracion Tiempo que tarda en ejectuarse el proceso en cpu
 * @param registros Registros  de la CPU.
 */
typedef struct
{
    int pid;
    int quantum;
    int duracion;
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


#endif
