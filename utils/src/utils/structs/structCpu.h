#ifndef CPU_STRUCT
#define CPU_STRUCT

#include <stdint.h>


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



#endif