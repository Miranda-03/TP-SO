#ifndef MEMORIA_INSTRUCCIONES
#define MEMORIA_INSTRUCCIONES

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/dictionary.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <commons/string.h>
#include <unistd.h>

/**
 * @fn    crear_mem_instrucciones
 * @brief crea un diccionario para guardar los PID y las instrucciones de cada proceso.
 */
void crear_mem_instrucciones();

/**
 * @fn    agregar_instrucciones
 * @brief agrega las instrucciones indicadas en el PATH con su PID al diccionario.
 */
int agregar_instrucciones(char *path, int pid);

/**
 * @fn    obtener_instruccion
 * @brief retorna una instruccion.
 */
char *obtener_instruccion(int pid, int pc);

/**
 * @fn    enviar_instruccion
 * @brief envia la instruccion a la CPU.
 */
void enviar_instruccion(int *socket, int pc, int pid);

/**
 * @fn    quitar_instrucciones
 * @brief quita el archivo con las instrucciones del proceso.
 */
int quitar_instrucciones(int pid);

#endif