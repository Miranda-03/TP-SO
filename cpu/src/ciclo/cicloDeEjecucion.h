#ifndef CICLO_H
#define CICLO_H

#include <connMemoria/connMemoria.h>
#include <commons/string.h>
#include <utils/enums/instrucciones.h>
#include <utils/structs/structSendContextCPU.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils/enums/motivosDesalojo.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <commons/log.h>
#include "setInstrucciones.h"
#include "MMU/MMU.h"
#include <semaphore.h>

/**
 * @fn    cicloDeEjecucion
 * @brief El ciclo de ejecución del CPU.
 */
void cicloDeEjecucion(int *CPUSocketMemoria,
                      int *CPUsocketBidireccionalDispatch,
                      int *CPUsocketBidireccionalInterrupt,
                      Contexto_proceso *procesoCPU,
                      int *interrupcion);

/**
 * @fn    execute
 * @brief Fase de execute del CPU.
 */
void execute(char **instruccionSeparada, Contexto_proceso *procesoCPU, char *instruccion, int *CPUsocketBidireccionalDispatch, t_log* loger);

/**
 * @fn    checkInterrupt
 * @brief verifica si hay una interrupcion con la variable pasada por parametro (int interrupcion).
 */
void checkInterrupt(Contexto_proceso *procesoCPU, int *CPUsocketBidireccionalDispatch);

/**
 * @fn    enviar_contexto_al_kernel
 * @brief envia el contexto al kernel junto con el motivo. Si el motivo es por I/O además se le envia la instrucción.
 */
void enviar_contexto_al_kernel(Contexto_proceso *procesoCPU, MotivoDesalojo motivo, char* instruccion, int *CPUsocketBidireccionalDispatch);

/**
 * @fn    agregar_registros_al_buffer
 * @brief agregar los registros del cpu al buffer.
 */
void agregar_registros_al_buffer(Contexto_proceso *procesoCPU, t_buffer *buffer);

char *mensaje_fetch_instruccion_log(int *pid, int *pc);
char *mensaje_execute_log(int *pid, char *instruccion);

/**
 * @fn    obtener_tam_pagina
 * @brief le pide a memoria el tamaño de la pagina.
 */
void obtener_tam_pagina (int socket);

void llega_proceso();

#endif