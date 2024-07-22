#ifndef CREAR_IO_H
#define CREAR_IO_H

#include <utils/enums/DispositivosIOenum.h>
#include <pthread.h>
#include "../Conexion/conectar.h"
#include <utils/enums/instruccionesIO.h>
#include <commons/config.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <commons/log.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include "../manage_io/funciones_DIALFS/funciones_DIALFS.h"

typedef struct
{
   char *identificador;
   char *config_path;
} moduloIO;

typedef struct
{
   int IO_Kernel_socket;
   int IO_Memoria_socket;
   moduloIO *modulo_io;
   TipoInterfaz tipo_interfaz;
} socket_hilo;

/**
 * @fn crearIO
 * @brief funcion para crear I/O.
 */
void crearIO(char *config_file, char *idIO, pthread_t *hilo_de_escucha, char *ip_kernel, char *ip_memoria);

/**
 * @fn hilo_conexion_io
 * @brief funcion del hilo de conexion.
 */
void *hilo_conexion_io(void *ptr);

/**
 * @fn instanciar_struct_io
 * @brief funcion para instancear un struct moduloIO.
 */
moduloIO *instanciar_struct_io(const char *identificador, const char *config_path);

/**
 * @fn destruir_struct_io
 * @brief destruye los char* dentro del struct para luego eliminarlo también.
 */
void destruir_struct_io(moduloIO *struct_io);

/**
 * @fn tipo_interfaz_del_config
 * @brief funcion para devolver el valor del tipo de interfaz del archivo de configuracion.
 */
TipoInterfaz tipo_interfaz_del_config(char *config_path);

/**
 * @fn manageGenerico
 * @brief funcion para el modulo del tipo GENERICO.
 */
void manageGenerico(moduloIO *modulo_io, int *socket, t_buffer *buffer_kernel, char *instruccion);

/**
 * @fn generar_struct_socket_hilo
 * @brief crear y llenar el struct para pasar como parametro a la función del hilo de conexión.
 * @param modulo_io struct moduloIO previamente creado.
 * @param IOsocketKernel socket comunicación del con el Kernel.
 * @param IOsocketMemoria socket comunicación del con la memoria.
 * @param interfaz tipo de interfaz.
 */
socket_hilo *generar_struct_socket_hilo(moduloIO * modulo_io, int *IOsocketKernel, int *IOsocketMemoria, TipoInterfaz interfaz);

/**
 * @fn *recibir_instruccion_del_kernel
 * @brief funcion para recibir la instruccion del kernel y devolver el buffer para leer los parametros luego.
 */
t_buffer *recibir_instruccion_del_kernel(char **instruccion, int *PID, int *socket);

/**
 * @fn *mensaje_info_operacion
 * @brief devulve el char* para el logger.
 */
char *mensaje_info_operacion(int PID, char *operacion);

/**
 * @fn *manageSTDIN
 * @brief funcion para las entradasalidas de tipo STDIN.
 */
void manageSTDIN(moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion, int pid);

void inicializarMutex();

void enviarMensajeAMemoria(int *socket, char *texto, int dir_fisica, int *resultado, t_buffer *buffer);

int esperarResultado(int *socket);

void manageSTDOUT(moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion, int pid);

TipoInterfaz tipo_interfaz_config(char *config_path);

char* obtener_nombre_archivo(char **comando);
int obtener_tamano_archivo(char **comando);
int obtener_puntero_archivo(char **comando);

void manageDialFS(int *pid, t_log *logger, moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion);

#endif
