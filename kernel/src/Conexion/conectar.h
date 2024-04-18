#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "kernel.config"
#define MAXCONN 10

#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/enums/TipoConnKernelCPU.h>

/**
* @fn    conectarModuloKernel
* @brief conecta el modulo kernel con los demas componentes.
*/
void conectarModuloKernel();

/**
* @fn    recibirClientes
* @brief función para recibir multiples clientes (multiplexación).
*/
void *recibirClientes(void *ptr);

/**
* @fn    atenderIO
* @brief función para atender al módulo I/O.
*/
void *atenderIO(void *socketComunicacion);

/**
* @fn    handshakeKernelCPU
* @brief función para atender realizar el handshake entre el kernel y el CPU.
*/
void handshakeKernelCPU(TipoConn conn);  

/**
* @fn    socketSegunConn
* @brief función para devolver el socket correspondiente al tipo de conexion.
*/
int socketSegunConn(TipoConn conn);

#endif