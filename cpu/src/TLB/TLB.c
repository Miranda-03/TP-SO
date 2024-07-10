#include "TLB.h"

t_queue *TLB;

AlgoritmoReemplazo_TLB algoritmo;

int cant_entradas_TLB;

int socket_con_memoria;

t_log *log_tlb;

void iniciar_TLB(int socket)
{
    TLB = queue_create();

    algoritmo = obtener_algoritmo_de_la_configuracion();

    cant_entradas_TLB = atoi(obtenerValorConfig("cpu.config", "CANTIDAD_ENTRADAS_TLB"));

    socket_con_memoria = socket;

    log_tlb = log_create("logs/cpu_tlb.log", "TLB", 1, LOG_LEVEL_INFO);
}

int obtener_algoritmo_de_la_configuracion()
{
    char *algoritmo_char = obtenerValorConfig("cpu.config", "ALGORITMO_TLB");

    if (strcmp(algoritmo_char, "FIFO"))
        return FIFO;
    else
        return LRU;
}

int obtener_marco(int num_pagina, int pid)
{
    int marco_tlb = -1;

    marco_tlb = buscar_en_TLB(num_pagina, pid);

    if (marco_tlb >= 0)
    {
        mensaje_tlb("HIT", num_pagina, pid);
        return marco_tlb;
    }

    mensaje_tlb("MISS", num_pagina, pid);

    marco_tlb = pedir_marco_a_memoria(num_pagina, pid);

    mensaje_obtener_marco(pid, num_pagina, marco_tlb);

    return marco_tlb;
}

void mensaje_obtener_marco(int pid, int num_pagina, int marco_tlb)
{
    char *mensaje = string_new();

    string_append(&mensaje, "PID: ");
    string_append(&mensaje, string_itoa(pid));
    string_append(&mensaje, " - OBTENER MARCO - Página: ");
    string_append(&mensaje, string_itoa(num_pagina));
    string_append(&mensaje, " - Marco: ");
    string_append(&mensaje, string_itoa(marco_tlb));

    log_info(log_tlb, mensaje);
}

void mensaje_tlb(char *estado, int num_pagina, int pid)
{
    char *mensaje = string_new();

    string_append(&mensaje, "PID: ");
    string_append(&mensaje, string_itoa(pid));
    string_append(&mensaje, " - TLB ");
    string_append(&mensaje, estado);
    string_append(&mensaje, " - Página: ");
    string_append(&mensaje, string_itoa(num_pagina));

    log_info(log_tlb, mensaje);
}

int pedir_marco_a_memoria(int num_pagina, int pid)
{
    t_buffer *buffer = buffer_create(8);
    buffer_add_uint32(buffer, pid);
    buffer_add_uint32(buffer, num_pagina);
    enviarMensaje(&socket_con_memoria, buffer, CPU, OBTENER_MARCO);

    TipoModulo *modulo = get_modulo_msg_recv(&socket_con_memoria);
    op_code *opcode = get_opcode_msg_recv(&socket_con_memoria);
    t_buffer *buffer_recv = buffer_leer_recv(&socket_con_memoria);

    int *marco = malloc(4);

    buffer_read(buffer_recv, marco, 4);

    buffer_destroy(buffer_recv);

    guardar_entrada_en_TLB(pid, num_pagina, *marco);

    int marco_return = *marco;

    free(marco);

    return marco_return;
}

void guardar_entrada_en_TLB(int pid, int num_pagina, int marco)
{
    Entrada_TLB *entrada = malloc(sizeof(Entrada_TLB));

    entrada->pid = pid;
    entrada->numero_de_pagina = num_pagina;
    entrada->marco = marco;

    queue_push(TLB, entrada);

    if (list_size(TLB->elements) > cant_entradas_TLB)
    {
        Entrada_TLB *entrada_sacar = queue_pop(TLB);
        free(entrada_sacar);
    }
}

int buscar_en_TLB(int num_pagina, int pid)
{
    int marco = -1;

    Entrada_TLB *entrada = NULL;

    bool buscar_marco(void *value)
    {
        Entrada_TLB *entrada = (Entrada_TLB *)value;
        if (entrada->pid == pid && entrada->numero_de_pagina == num_pagina)
        {
            marco = entrada->marco;
            if (algoritmo == LRU)
                return 1;
        }
        return 0;
    }

    entrada = list_remove_by_condition(TLB->elements, buscar_marco);

    if (entrada != NULL)
        queue_push(TLB, entrada);

    return marco;
}