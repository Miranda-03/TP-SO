#ifndef CPU_STRUCT
#define CPU_STRUCT

#include <stdint.h>


/**
 * @brief struct de registros que se mandan a la cpu
 */

typedef struct {
    int32_t i32;
    uint8_t u8;
} Registro;

typedef struct {
    Registro ax;
    Registro eax;
    Registro bx;
    Registro ebx;
    Registro cx;
    Registro ecx;
    Registro dx;
    Registro edx;
} Registros;

#endif