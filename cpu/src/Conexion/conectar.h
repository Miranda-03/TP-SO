#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "cpu.config"
#define MAXCONN 1

#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <utils/enums/codigosOperacion.h>
#include <utils/enums/TipoConnKernelCPU.h>
#include <pthread.h>

/**
* @fn    conectarModuloCPU
* @brief conecta el modulo CPU con los demas componentes.
*/
void conectarModuloCPU();

/**
* @fn    handshakeCPUMemoria
* @brief hacer handshake CPU memoria.
*/
void handshakeCPUMemoria();

/**
* @fn    recibirConn
* @brief recibir y gestionar conexion entrante.
*/
void recibirConn(int *socket, TipoConn conexion);

/**
* @fn    manageKernel
* @brief gestionar la conexion con Kernel.
*/
void manageKernel(int *socket, TipoConn conexion);


#endif