#ifndef P_CONTEXT_SEND_H
#define P_CONTEXT_SEND_H

#include <stdint.h>
#include "structCpu.h"


/**
 * @brief struct contexto del proceso enviado desde el Kernel hacia el CPU
 */
typedef struct
{
    int pid;
    uint32_t pc;
    Registros registros;
} Contexto_proceso;



#endif