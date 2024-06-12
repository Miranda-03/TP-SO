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
    IO_GEN_SLEEP_CPU,
    MOV_IN,
    MOV_OUT,
    RESIZE,
    COPY_STRING,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    EXIT
} instruccion;

#endif