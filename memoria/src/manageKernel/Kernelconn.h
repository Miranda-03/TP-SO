#ifndef CONN_KERNEL
#define CONN_KERNEL

#include <utils/structs/structSendRecvMSG.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <mem_instrucciones/mem_instrucciones.h>
#include <mem_usuario/mem_usuario.h>

/**
* @fn    manage_conn_kernel
* @brief funcion del hilo de conexion con el Kernel.
*/
void *manage_conn_kernel(void *ptr);

/**
* @fn    obtener_pid
* @brief obtener el PID del stream.
*/
unsigned int obtener_elpid(void *stream);

/**
* @fn    obtener_path_instruccion
* @brief obtener el PATH del archivo de instrucciones del stream.
*/
char* obtener_path_instruccion(t_buffer *buffer);

/**
 * @fn    guardar_nuevo_proceso
 * @brief crea las estructuras necesarias para el nuevo proceso.
 */
int guardar_nuevo_proceso(char *path, int pid);

/**
* @fn    obtener_instuccion_kernel
* @brief obtener la instruccion del Kernel en el stream.
*/
unsigned int obtener_instuccion_kernel(void *stream);

/**
* @fn    enviar_mensaje_de_confirmacion
* @brief envia un mensaje al Kernel indicando exito o error al guardar las instrucciones.
*/
void enviar_mensaje_de_confirmacion(int *socket, int instruccion_guardada);

#endif