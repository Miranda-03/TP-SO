#ifndef SET_INSTRUCCIONES_H
#define SET_INSTRUCCIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/structs/structSendContextCPU.h>
#include <string.h>
#include <utils/structs/structCpu.h>

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
 * @fn    obtenerRegistro
 * @brief obtiene el registro según un dato tipo char*.
 */
Registro *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU, char *tipo);

#endif