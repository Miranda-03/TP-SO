#ifndef CPU_STRUCT
#define CPU_STRUCT

#include <stdint.h>


/**
 * @brief struct de registros que se mandan a la cpu
 */

typedef union {
    int32_t i32;
    uint8_t u8;
} Registro;

typedef struct {
    uint32_t pc;
    Registro ax;
    Registro eax;
    Registro bx;
    Registro ebx;
    Registro cx;
    Registro ecx;
    Registro dx;
    Registro edx;
} Registros;

/*
typedef struct {
    Registros registros;
} Contexto_proceso;
*/


#endif