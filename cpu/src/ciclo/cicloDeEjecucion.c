#include "cicloDeEjecucion.h"

int *interrupcion_ce;

void cicloDeEjecucion(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion_main)
{
    t_log *loger = log_create("logs/cpu.log", "Ciclo CPU", 1, LOG_LEVEL_INFO);
    interrupcion_ce = interrupcion_main;

    while (1)
    {
        if ((procesoCPU->pid) != -1)
        {
            // FETCH
            log_info(loger, mensaje_fetch_instruccion_log(&(procesoCPU->pid), &(procesoCPU->pc)));
            char *instruccion = recibirInstruccion(CPUSocketMemoria, procesoCPU->pid, procesoCPU->pc);

            procesoCPU->pc += 1;

            // DECODE

            char **instruccionSeparada = string_split(instruccion, " ");

            execute(instruccionSeparada, procesoCPU, instruccion, CPUsocketBidireccionalDispatch, loger);

            checkInterrupt(procesoCPU, CPUsocketBidireccionalDispatch);
        }
    }
}

void execute(char **instruccionSeparada, Contexto_proceso *procesoCPU, char *instruccion, int *CPUsocketBidireccionalDispatch, t_log *loger)
{
    char *operacion = instruccionSeparada[0];
    char *primerParametro = instruccionSeparada[1];
    char *segundoParametro = instruccionSeparada[2];
    int *registro;
    Registro *reg;
    char tipo;

    if (strcmp(operacion, "SET") == 0)
    {
        log_info(loger, mensaje_execute_log(&(procesoCPU->pid), instruccion));
        instruccion_SET(primerParametro, segundoParametro, procesoCPU, &tipo, reg);
    }
    else if (strcmp(operacion, "SUM") == 0)
    {
        log_info(loger, mensaje_execute_log(&(procesoCPU->pid), instruccion));
        instruccion_SUM(primerParametro, segundoParametro, procesoCPU, &tipo, reg);
    }
    else if (strcmp(operacion, "SUB") == 0)
    {
        log_info(loger, mensaje_execute_log(&(procesoCPU->pid), instruccion));
        instruccion_SUB(primerParametro, segundoParametro, procesoCPU, &tipo, reg);
    }
    else if (strcmp(operacion, "JNZ") == 0)
    {
        log_info(loger, mensaje_execute_log(&(procesoCPU->pid), instruccion));
        instruccion_JNZ(primerParametro, segundoParametro, procesoCPU, &tipo, reg);
    }
    else if (strcmp(operacion, "IO_GEN_SLEEP") == 0)
    {
        log_info(loger, mensaje_execute_log(&(procesoCPU->pid), instruccion));
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_IO, instruccion, CPUsocketBidireccionalDispatch);
    }
    else if (strcmp(operacion, "EXIT") == 0)
    {
        log_info(loger, mensaje_execute_log(&(procesoCPU->pid), instruccion));
        enviar_contexto_al_kernel(procesoCPU, EXIT_SIGNAL, NULL, CPUsocketBidireccionalDispatch);
        // log_destroy(loger_execute);
    }
}

void checkInterrupt(Contexto_proceso *procesoCPU, int *CPUsocketBidireccionalDispatch)
{
    if (procesoCPU->pid == -1)
        return;

    if (*interrupcion_ce == 1)
    {
        *interrupcion_ce = 0;
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_KERNEL, NULL, CPUsocketBidireccionalDispatch);
    }
    else if (*interrupcion_ce == 2)
    {
        *interrupcion_ce = 0;
        enviar_contexto_al_kernel(procesoCPU, FIN_DE_QUANNTUM, NULL, CPUsocketBidireccionalDispatch);
    }
}

void enviar_contexto_al_kernel(Contexto_proceso *procesoCPU, MotivoDesalojo motivo, char *instruccion, int *CPUsocketBidireccionalDispatch)
{
    t_buffer *buffer;
    if (instruccion != NULL)
    {
        buffer = buffer_create((sizeof(uint32_t) * 10) + strlen(instruccion) + 1 + sizeof(MotivoDesalojo));
    }
    else
    {
        buffer = buffer_create(sizeof(uint32_t) * 9 + sizeof(MotivoDesalojo));
    }
    buffer_add_uint32(buffer, motivo); // No estoy seguto si era un uint32_t
    buffer_add_uint32(buffer, procesoCPU->pid);
    buffer_add_uint32(buffer, procesoCPU->pc);
    agregar_registros_al_buffer(procesoCPU, buffer);

    if (instruccion != NULL)
    { // No estoy seguro de la comparacion
        buffer_add_string(buffer, strlen(instruccion) + 1, instruccion);
    }

    enviarMensaje(CPUsocketBidireccionalDispatch, buffer, CPU, MENSAJE);

    procesoCPU->pid = -1;
    *interrupcion_ce = 0;
}

void agregar_registros_al_buffer(Contexto_proceso *procesoCPU, t_buffer *buffer)
{
    buffer_add_uint8(buffer, procesoCPU->registros.ax.u8);
    buffer_add_uint32(buffer, procesoCPU->registros.eax.i32);
    buffer_add_uint8(buffer, procesoCPU->registros.bx.u8);
    buffer_add_uint32(buffer, procesoCPU->registros.ebx.i32);
    buffer_add_uint8(buffer, procesoCPU->registros.cx.u8);
    buffer_add_uint32(buffer, procesoCPU->registros.ecx.i32);
    buffer_add_uint8(buffer, procesoCPU->registros.dx.u8);
    buffer_add_uint32(buffer, procesoCPU->registros.edx.i32);
}

char *mensaje_fetch_instruccion_log(int *pid, int *pc)
{
    char *result = string_new();
    string_append(&result, "PID: ");
    string_append(&result, string_itoa(*pid));
    string_append(&result, " - FETCH - Program Counter: ");
    string_append(&result, string_itoa(*pc));
    return result;
}

char *mensaje_execute_log(int *pid, char *instruccion)
{
    char *result = string_new();
    string_append(&result, "PID: ");
    string_append(&result, string_itoa(*pid));
    string_append(&result, " - Ejecutando: ");
    if (strcmp(string_split(instruccion, " ")[0], "EXIT") == 0)
    {
        string_append(&result, "EXIT");
    }
    else
    {
        char **instruccionSeparada = string_n_split(instruccion, 2, " ");
        string_append(&result, instruccionSeparada[0]);
        string_append(&result, " - ");
        string_append(&result, instruccionSeparada[1]);
    }

    return result;
}