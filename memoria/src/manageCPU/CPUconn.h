#ifndef CONN_CPU
#define CONN_CPU

#include <utils/structs/structSendRecvMSG.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <mem_instrucciones/mem_instrucciones.h>

/**
* @fn    manage_conn_cpu
* @brief funcion del hilo de conexion con el CPU.
*/
void *manage_conn_cpu(void *ptr);

/**
* @fn    obtener_pid
* @brief retorna el PID del stream del buffer.
*/
//unsigned int obtener_pid(void *stream);

/**
* @fn    obtener_pc
* @brief retorna el PC del stream del buffer.
*/
//unsigned int obtener_pc(void *stream);

/**
* @fn    enviar_instruccion
* @brief buscar y enviar la instruccion al CPU.
*/
void enviar_instruccion(int *socket, unsigned int pc, unsigned int pid);

//int acceso_tabla_de_pagina(Memoria* memoria, int numero_pagina);

#endif