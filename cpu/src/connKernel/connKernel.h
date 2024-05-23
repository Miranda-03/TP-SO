#ifndef KERNEL_CONN_H
#define KERNEL_CONN_H

#include <stdint.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>

/**
 * @brief struct para pasar los parametros a la funcion del hilo
 */
typedef struct
{
    int *socket;
    Proceso *procesosCPU;
} parametros_hilo;  

/**
* @fn    manageDISPATCH
* @brief Función en el hilo para la conexión dispatch con el Kernel.
*/
void * manageDISPATCH(void * ptr)

/**
* @fn    manageINTERRUPT
* @brief Función en el hilo para la conexión interrumpt con el Kernel.
*/
void * manageINTERRUPT(void * ptr)

#endif