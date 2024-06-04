
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
void execute(char instruccionSeparada[], Contexto_proceso *procesoCPU, char* instruccion, int *CPUsocketBidireccionalDispatch );

/**
 * @fn    obtenerRegistro
 * @brief obtiene el registro según un dato tipo char*.
 */
int* obtenerRegistro(char* registro, Contexto_proceso *procesoCPU);

/**
 * @fn    checkInterrupt
 * @brief verifica si hay una interrupcion con la variable pasada por parametro (int interrupcion).
 */
void checkInterrupt(Contexto_proceso *procesoCPU, int *interrupcion, int *CPUsocketBidireccionalDispatch);

/**
 * @fn    enviar_contexto_al_kernel
 * @brief envia el contexto al kernel junto con el motivo. Si el motivo es por I/O además se le envia la instrucción.
 */
void enviar_contexto_al_kernel(Contexto_proceso *procesoCPU, MotivoDesalojo motivo, char* instruccion, int *CPUsocketBidireccionalDispatch);

/**
 * @fn    instruccion_JNZ
 * @brief ejecutar la instrucción JNZ.
 */
void instruccion_JNZ(Contexto_proceso *procesoCPU, int *registro, int valor);

/**
 * @fn    agregar_registros_al_buffer
 * @brief agregar los registros del cpu al buffer.
 */
void agregar_registros_al_buffer(Contexto_proceso *procesoCPU, t_buffer *buffer);

#endif
