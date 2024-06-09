#include "structSendContextCPU.h"
#include "utils/enums/motivosDesalojo.h"
#include "utils/enums/DispositivosIOenum.h"
#include "utils/enums/instruccionesIO.h"

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
    instruccionIO* instruccion;
} parametros_hilo_Kernel;  

/**
 * @brief struct para pasar los parametros a la funcion del hilo
 */

typedef struct 
{
    int *socket;
    TipoInterfaz* interfaz;
    char* identificador;
}parametros_hilo_IO;
