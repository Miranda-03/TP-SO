#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <utils/structs/structProcesos.h>
#include <utils/enums/motivosDesalojo.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include <utils/enums/codigosOperacion.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>

typedef struct
{
    t_queue *cola_de_ready;
    t_list *lista_bloqueados_generico;
    t_list *lista_bloqueados_STDIN;
    t_list *lista_bloqueados_STDOUT;
    t_list *lista_bloqueados_DialFS;
} colasCortoPlazo;

typedef struct
{
    int KernelSocketCPUDispatch;
    int KernelSocketCPUInterrumpt;
    t_dictionary *interfaces_conectadas;
    colasCortoPlazo *colasCP;
} ParamsPCP;

typedef struct
{
    MotivoDesalojo motivo;
    char *instruccion;
    Pcb *pcb;
} MensajeProcesoDelCPU;

/**
* @fn    planificarCortoPlazo
* @brief planificador de corto plazo.
*/
void *planificarCortoPlazo(void *ptr);

/**
* @fn    escuchaDispatch
* @brief funcion para del hilo para esuchar al modulo CPU en espera de recibir el proceso junto a mas informacion necesaria.
*/
void *escuchaDispatch(void *ptr);

#endif