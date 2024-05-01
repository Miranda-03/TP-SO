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
 * @fn instanciar_struct_io
 * @brief funcion para instancear un struct moduloIO.
 */
moduloIO *instanciar_struct_io(char *identificador, char *config_path);

/**
 * @fn tipo_interfaz_del_config
 * @brief funcion para devolver el valor del tipo de interfaz del archivo de configuracion.
 */
TipoInterfaz *tipo_interfaz_del_config(char *config_path);

/**
 * @fn manageGenerico
 * @brief funcion para el modulo del tipo GENERICO.
 */
void manageGenerico(moduloIO *modulo_io, int *socket);

#endif
