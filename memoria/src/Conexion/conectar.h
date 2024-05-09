#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "memoria.config"
#define MAXCONN 10

#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <manageCPU/CPUconn.h>
#include <manageKernel/Kernelconn.h>

/**
* @fn    conectarModuloMemoria
* @brief conecta el modulo memoria con los demas componentes.
*/
void conectarModuloMemoria();

/**
* @fn    recibirModulo
* @brief función para recibir multiples clientes (multiplexación).
*/
void *recibirModulo(void *ptr);

/**
* @fn    atenderModulo
* @brief función para atender al módulo cliente.
*/
void *atenderModulo(void *socketComunicacion);

/**
* @fn    manageIO
* @brief manejar el handshake del modulo IO.
*/
void manageIO(int *socket);

/**
* @fn    manageModulo
* @brief manejar el handshake del modulo.
*/
void manageModulo(int *socket, TipoModulo modulo);

/**
* @fn    iniciar_hilo_conexion_cpu
* @brief funcion para inicar el hilo de ejecucion para enviar y recibir mensajes con el modulo correspondiente.
*/
void iniciar_hilo_conexion(int *socket, TipoModulo modulo);

#endif