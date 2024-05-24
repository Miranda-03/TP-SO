#ifndef KERNEL_CONN_H
#define KERNEL_CONN_H

#include <stdint.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/structs/structSendContextCPU.h>

/**
 * @brief struct para pasar los parametros a la funcion del hilo
 */
typedef struct
{
    int *socket;
    int *interrupcion;
    Contexto_proceso *procesosCPU;
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

/**
* @fn    obtener_procesoCPU_del_stream
* @brief obtener el pcb del stream enviado desde el Kernel.
*/
void obtener_procesoCPU_del_stream(void* stream, Contexto_proceso *procesoCPU);

/**
* @fn    obtener_registros
* @brief obtener los registros del stream.
*/
void obtener_registros(void* stream, unsigned int *offset ,Contexto_proceso *procesoCPU);

#endif