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
#include <pthread.h>
#include <utils/enums/TipoConnKernelCPU.h>
#include <utils/structs/structSendContextCPU.h>
#include <stdint.h>
#include <connKernel/connKernel.h>


/**
 * @fn    conectarModuloCPU
 * @brief conecta el modulo CPU con los demas componentes.
 */
void conectarModuloCPU(int *no,
                       int *CPUsocketBidireccionalDispatch,
                       int *CPUsocketBidireccionalInterrupt,
                       Contexto_proceso *procesoCPU,
                       int *interrupcion);

/**
 * @fn    handshakeCPUMemoria
 * @brief hacer handshake CPU memoria.
 */
void handshakeCPUMemoria(int *CPUSocketMemoria);

/**
 * @fn    recibirConn
 * @brief recibir y gestionar conexion entrante.
 */
void recibirConn(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion);

/**
 * @fn    manageKernel
 * @brief gestionar la conexion con Kernel.
 */
void manageKernel(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion);

/**
 * @fn    crearHiloDISPATCH
 * @brief crear hilo con la función de conexión dispatch con el Kernel.
 */
void crearHiloDISPATCH(int *socket, Contexto_proceso *procesoCPU);

/**
 * @fn    crearHiloINTERRUPT
 * @brief crear hilo con la función de conexión interrumpt con el Kernel.
 */
void crearHiloINTERRUPT(int *socket , int *interrupcion);

#endif