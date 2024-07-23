#ifndef MEMORIA_USUARIO
#define MEMORIA_USUARIO

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <utils/enums/DispositivosIOenum.h>
#include <commons/string.h>
#include <unistd.h>
#include <string.h> 
#include <commons/log.h>

typedef struct
{
    int numero_de_marco;
    int ocupado;
} Marco;

typedef struct
{
    int numero_de_pagina;
    Marco *marco;
} Pagina;

/**
 * @fn    crear_tabla_de_pagina
 * @brief crea la tabla de paginas vacia del proceso y la agrega al diccionario de tablas de paginas.
 */
void crear_tabla_de_pagina(int PID);

/**
 * @fn    iniciar_espacio_usuario
 * @brief hace un malloc para iniciar el espacio de usuario representado por un void.
 */
void iniciar_espacio_usuario(char *path_config);

/**
 * @fn    iniciar_marcos
 * @brief inicia los marcos y los marca como desocupados.
 */
void iniciar_marcos();

/**
 * @fn    iniciar_tablas_de_paginas
 * @brief crea el diccionario en donde se van a guardar las tablas de paginas de los procesos.
 */
void iniciar_tablas_de_paginas();

/**
 * @fn    resize_proceso
 * @brief hace el rezise del proceso agragando o quitando paginas.
 */
void resize_proceso(int PID, int tam, int enviar_dato);

/**
 * @fn    encontrar_marco
 * @brief encuentra el marco segun la pagina solicitada.
 */
void encontrar_marco(int PID, int numero_de_pagina);

/**
 * @fn    escribir_memoria
 * @brief escribe en la memoria los bytes indicados en la direccion fisica pasada por parametro (hace un memcpy en espacio).
 */
void escribir_memoria(int PID, int direccion_fisica, int bytes_a_escribir, void *dato, int *socket);

/**
 * @fn    leer_memoria
 * @brief lee en la memoria los bytes indicados.
 */
void leer_memoria(int PID, int direccion_fisica, int bytes_a_leer, int *socket);

/**
 * @fn    quitar_paginas
 * @brief quita tantas paginas como fuese necesario para ajustarse al tamaño requerido.
 */
int quitar_paginas(int PID, int tam, int cant_paginas);

/**
 * @fn    agregar_paginas
 * @brief agrega tantas paginas como fuese necesario para ajustarse al tamaño requerido.
 */
int agregar_paginas(int PID, int tam, int cant_paginas);

/**
 * @fn    cant_paginas_por_proceso
 * @brief devulve la cantidad de paginas que tiene un proceso.
 */
int cant_paginas_por_proceso(int PID);

/**
 * @fn    enviar_dato_a_CPU
 * @brief envia el dato solicitado al CPU por el socket correspondiente.
 */
void enviar_dato_a_modulo(void *dato, int bytes_a_enviar, int *socket);

/**
 * @fn    obtener_socket
 * @brief guardar el socket en la variable global de la memoria de usuario.
 */
void obtener_socket(int *socket_main);

/**
 * @fn    obtener_socket_stdout
 * @brief guardar el socket en la variable global de la memoria de usuario.
 */
void obtener_socket_stdout(int *socket_io);

/**
 * @fn    obtener_socket_stdin
 * @brief guardar el socket en la variable global de la memoria de usuario.
 */
void obtener_socket_stdin(int *socket_io);

/**
 * @fn    enviar_tam_de_pagina
 * @brief le envia al CPU el tamaño de las paginas.
 */
void enviar_tam_de_pagina();

int *obtener_socket_correspondiente(TipoInterfaz *interfaz);

Marco *encontrar_marco_libre();

/**
 * @fn    liberar_memoria
 * @brief libera los marcos del proceso si tiene.
 */
int liberar_memoria(int pid);

/**
 * @fn    quitar_tabla_de_pagina
 * @brief quita la tabla de paginas del proceso.
 */
void quitar_tabla_de_pagina(int pid);

#endif