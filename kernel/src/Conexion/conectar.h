#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "kernel.config"
#define MAXCONN 10

#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/enums/TipoConnKernelCPU.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/codigosOperacion.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/structs/structInt.h>
#include "Interfaces/interfaces.h"


/**
* @fn    conectarModuloKernel
* @brief conecta el modulo kernel con los demas componentes.
*/
void conectarModuloKernel(int *KernelSocketMemoria, int *KernelSocketCPUDispatch, int *KernelSocketCPUInterrumpt, t_dictionary *interfaces_conectadas, char *path_config);

/**
* @fn    manageCPU
* @brief la conexion con el CPU en un hilo.
*/
void manageCPU(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion);

/**
* @fn    crearHiloDISPATCH
* @brief conexion dispatch en un hilo.
*/
void crearHiloDISPATCH(int *socket, MotivoDesalojo *motivo, int *pid, Registros *registros, instruccionIO *instruccion);

/**
* @fn    recibirClientes
* @brief función para recibir multiples clientes (multiplexación).
*/
void *recibirClientes(void *ptr);

/**
* @fn    atenderIO
* @brief función para atender al módulo I/O.
*/
void *atenderIO(void *ptr);

/**
* @fn    handshakeKernelCPU
* @brief función para atender realizar el handshake entre el kernel y el CPU.
*/
void handshakeKernelCPU(TipoConn conn, int *socket);  

/**
* @fn    handshakeKernelMemoria
* @brief función para atender realizar el handshake entre el kernel y el Memoria.
*/
void handshakeKernelMemoria(int* socketMemoria); 

/**
* @fn    socketSegunConn
* @brief función para devolver el socket correspondiente al tipo de conexion.
*/
int socketSegunConn(TipoConn conn);

/**
* @fn    manageIO
* @brief manejar el handshake del modulo IO.
*/
void manageIO(int *socket);

/**
* @fn    obtener_identificador
* @brief obtener el identificador de la interfaz.
*/
//char *obtener_identificador(void *stream);

void recibirConexion(int *socket, TipoConn conexion);

char *get_tipo_conn(TipoConn conn);

void obtener_ip_de_modulo_memoria(int socket, t_config *config);

void *hilo_responder_ips(void *ptr);

#endif