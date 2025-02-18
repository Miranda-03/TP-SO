#ifndef CONN_CPU
#define CONN_CPU

#include <utils/structs/structSendRecvMSG.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <mem_instrucciones/mem_instrucciones.h>
#include <mem_usuario/mem_usuario.h>

/**
* @fn    manage_conn_cpu
* @brief funcion del hilo de conexion con el CPU.
*/
void *manage_conn_cpu(void *ptr);

#endif