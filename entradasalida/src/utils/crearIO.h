#ifndef CREAR_IO_H
#define CREAR_IO_H

#include <utils/enums/DispositivosIOenum.h>
#include <pthread.h>
#include "../Conexion/conectar.h"
#include <utils/enums/instruccionesIO.h>
#include "commons/config.h"
#include <utils/funcionesBuffer/funcionesBuffer.h>

typedef struct
{
   char *identificador;
   char *config_path;
} moduloIO;

typedef struct
{
   int *IO_Kernel_socket;
   int *IO_Memoria_socket;
   moduloIO *modulo_io;
   TipoInterfaz tipo_interfaz;
} socket_hilo;

/**
 * @fn crearIO
 * @brief funcion para crear I/O.
 */
void crearIO();

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
void manageGenerico(moduloIO *modulo_io, int *socket);

/**
 * @fn generar_struct_socket_hilo
 * @brief crear y llenar el struct para pasar como parametro a la función del hilo de conexión.
 * @param modulo_io struct moduloIO previamente creado.
 * @param IOsocketKernel socket comunicación del con el Kernel.
 * @param IOsocketMemoria socket comunicación del con la memoria.
 * @param interfaz tipo de interfaz.
 */
socket_hilo *generar_struct_socket_hilo(moduloIO * modulo_io, int *IOsocketKernel, int *IOsocketMemoria, TipoInterfaz interfaz);

#endif
