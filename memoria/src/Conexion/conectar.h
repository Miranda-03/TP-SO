#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "memoria.config"
#define MAXCONN 3

#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/enums/ModulosEnum.h>
#include "utils.h"

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

#endif