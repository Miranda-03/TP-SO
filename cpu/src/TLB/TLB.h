#ifndef TLB_H
#define TLB_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <commons/collections/queue.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <commons/log.h>
#include <commons/string.h>

typedef struct
{
    int pid;
    int numero_de_pagina;
    int marco;
} Entrada_TLB;

typedef enum
{
    FIFO,
    LRU
} AlgoritmoReemplazo_TLB;

/**
* @fn    iniciar_TLB
* @brief inicia la cola que representa la TLB y demas datos necesarios para su funcionamiento.
*/
void iniciar_TLB(int socket);

/**
* @fn    obtener_algoritmo_de_la_configuracion
* @brief obtiene el algoritmo de reemplazo de la TLB.
*/
int obtener_algoritmo_de_la_configuracion();

/**
* @fn    obtener_marco
* @brief obtiene el marco en la TLB o se lo pide a la memoria.
*/
int obtener_marco(int num_pagina, int pid);

/**
* @fn    pedir_marco_a_memoria
* @brief le pide el marco a la memoria.
*/
int pedir_marco_a_memoria(int num_pagina, int pid);

/**
* @fn    guardar_entrada_en_TLB
* @brief guarda una nueva entrada de la TLB.
*/
void guardar_entrada_en_TLB(int pid, int num_pagina, int marco);

/**
* @fn    buscar_en_TLB
* @brief busca si la pagina del proceso correspondiente se encentra guardada en la TLB.
*/
int buscar_en_TLB(int num_pagina, int pid);


void mensaje_obtener_marco(int pid, int num_pagina, int marco_tlb);

void mensaje_tlb(char *estado, int num_pagina, int pid);

#endif