#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "kernel.config"
#define MAXCONN 1

#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/enums/ModulosEnum.h>

/**
* @fn    conectarModuloKernel
* @brief conecta el modulo memoria con los demas componentes.
*/
void conectarModuloKernel();

/**
* @fn    recibirModulo
* @brief función para recibir multiples clientes (multiplexación).
*/
void *recibirClientes(void *ptr);

/**
* @fn    atenderModulo
* @brief función para atender al módulo cliente.
*/
void *atenderIo(void *socketComunicacion);

/**
* @fn    manageIO
* @brief manejar el handshake del modulo IO.
*/
void manageIO(int *socket,  t_buffer *buffer, t_resultHandShake  *result);



#endif /* CONECTAR_F */