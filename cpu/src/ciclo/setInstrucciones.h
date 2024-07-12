#ifndef SET_INSTRUCCIONES_H
#define SET_INSTRUCCIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/structs/structSendContextCPU.h>
#include <string.h>
#include <utils/structs/structCpu.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include "cicloDeEjecucion.h"
#include "connMemoria/connMemoria.h"
#include <commons/string.h>
#include <utils/enums/DispositivosIOenum.h>
/**
 * @fn    instruccion_SUM
 * @brief realiza la instruccion SUM.
 */
void instruccion_SUM(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg);

/**
 * @fn    instruccion_SET
 * @brief realiza la instruccion SET.
 */
void instruccion_SET(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg);

/**
 * @fn    instruccion_SUB
 * @brief realiza la instruccion SUB.
 */
void instruccion_SUB(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg);

/**
 * @fn    instruccion_JNZ
 * @brief realiza la instruccion JNZ.
 */
void instruccion_JNZ(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg);

/**
 * @fn    instruccion_RESIZE
 * @brief realiza la instruccion RESIZE.
 */
void instruccion_RESIZE(char *primerParametro, Contexto_proceso *procesoCPU, int socket, int *socketDispatch);

/**
 * @fn    instruccion_COPY_STRING
 * @brief realiza la instruccion COPY_STRING.
 */
void instruccion_COPY_STRING(char *primerParametro, Contexto_proceso *procesoCPU, int socket_memoria);

/**
 * @fn    instruccion_MOV_IN
 * @brief realiza la instruccion MOV_IN.
 */
void instruccion_MOV_IN(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg, int socket_memoria);

/**
 * @fn    instruccion_MOV_OUT
 * @brief realiza la instruccion MOV_OUT.
 */
void instruccion_MOV_OUT(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg, int socket_memoria);

/**
 * @fn    instruccion_IO_STD
 * @brief realiza la instruccion IO_STDIN_READ y IO_STDOUT_WRITE.
 */
void instruccion_IO_STD(char **instruccion, Contexto_proceso *procesoCPU, char *tipo, Registro *reg, int socket_memoria, int socket_dispatch, TipoInterfaz interfaz);
/**
 * @fn    obtenerRegistro
 * @brief obtiene el registro según un dato tipo char*.
 */
Registro *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU, char *tipo);

void instruccion_IO_FS_TRUNCATE(char *operacion, char *id_io, char *nombre_archivo, char *reg_tam, char *tipo, Registro *reg, int socket_dispatch, Contexto_proceso *procesoCPU);

char *obtener_array_de_direcciones(Contexto_proceso *procesoCPU, char *tipo, Registro *reg, char *dir, char *tam);

void instruccion_IO_FS_WRITE_READ(char *operacion, char *id_io, char *nombre_archivo, char *direccion, char *reg_tam, char *puntero, char *tipo, Registro *reg, int socket_dispatch, Contexto_proceso *procesoCPU);

#endif