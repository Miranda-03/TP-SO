#include "structSendContextCPU.h"
#include "utils/enums/motivosDesalojo.h"

/**
 * @brief struct para pasar los parametros a la funcion del hilo
 */
typedef struct
{
    int *socket;
    int *interrupcion;
    Contexto_proceso *procesoCPU;
} parametros_hilo_Cpu;  

/**
 * @brief struct para pasar los parametros a la funcion del hilo
 */
typedef struct
{
    int *socket;
    int *pid;
    MotivoDesalojo *motivo;
    Registros* registros;
    char** instruccion;
} parametros_hilo_Kernel;  