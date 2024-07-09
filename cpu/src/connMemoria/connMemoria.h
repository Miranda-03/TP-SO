#ifndef CONN_MEMORIA_H
#define CONN_MEMORIA_H

#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include "MMU/MMU.h"
#include <commons/string.h>
#include <commons/log.h>

/**
 * @fn    recibirInstruccion
 * @brief obtiene la instruccion de la memoria.
 */
char *recibirInstruccion(int *socket, unsigned int pid, unsigned int pc);

void *cpu_leer_memoria(int direccion_logica_inicio, int bytes_a_leer, int pid, int socket_memoria);

int escribir_memoria(int direccion_logica_inicio, int bytes_a_escribir, int pid, void *dato, int socket_memoria);

void iniciar_loger_conn_memoria();

void mensaje_conn_memoria(int pid, char *accion, char *direccion_fisica, void *dato);

#endif