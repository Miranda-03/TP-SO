#ifndef INS_ENUM
#define INS_ENUM

/**
 * @brief enum para las instrucciones generales.
 * @param SET
 * @param SUM
 * @param SUB
 * @param JNZ
 */

typedef enum
{
    SET,
    SUM,
    SUB,
    JNZ,
    IO_GEN_SLEEP,
    EXIT
} instruccion;

#endif