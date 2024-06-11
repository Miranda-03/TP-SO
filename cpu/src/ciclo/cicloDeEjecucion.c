#include "cicloDeEjecucion.h"

void cicloDeEjecucion(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion)
{
    t_log *loger = log_create("logs/cpu.log", "Ciclo CPU", 1, LOG_LEVEL_INFO);

    while (1)
    {
        if ((procesoCPU->pid) != -1)
        {
            // FETCH
            log_info(loger, mensaje_fetch_instruccion_log(&(procesoCPU->pid), &(procesoCPU->registros.pc)));
            char *instruccion = recibirInstruccion(CPUSocketMemoria, procesoCPU->pid, procesoCPU->registros.pc);

            procesoCPU->pc= 1;

            // DECODE

            char **instruccionSeparada = string_split(instruccion, " ");

            execute(instruccionSeparada, procesoCPU, instruccion, CPUsocketBidireccionalDispatch);

            checkInterrupt(procesoCPU, interrupcion, CPUsocketBidireccionalDispatch);
        }
    }
}

void execute(char **instruccionSeparada, Contexto_proceso *procesoCPU, char *instruccion, int *CPUsocketBidireccionalDispatch)
{
    char *operacion = instruccionSeparada[0];
    char *primerParametro = instruccionSeparada[1];
    char *segundoParametro = instruccionSeparada[2];
    int *registro;
    Registro *reg;
    char tipo;
    t_log *loger_execute = log_create("logs/cpu_execute.log", "Ciclo CPU", 1, LOG_LEVEL_INFO);

    log_info(loger_execute, mensaje_execute_log(&(procesoCPU->pid), instruccion));

    switch (*operacion)
    {
    case SET:
        reg = obtenerRegistro(primerParametro, procesoCPU, &tipo);
        if (tipo == 'i')
        {
            printf("Valor de ax: %d\n", reg->i32);
            reg->i32 = atoi(segundoParametro);
        }
        else if (tipo == 'u')
        {
            reg->u8 = atoi(segundoParametro);
        }
        *registro = atoi(segundoParametro);
        break;
    case SUM:
        reg = obtenerRegistro(primerParametro, procesoCPU, &tipo);
        if (tipo == 'i')
        {
            printf("Valor de ax: %d\n", reg->i32);
            reg->i32 += atoi(segundoParametro);
        }
        else if (tipo == 'u')
        {
            reg->u8 += atoi(segundoParametro);
        }
        break;
    case SUB:
        reg = obtenerRegistro(primerParametro, procesoCPU, &tipo);
        if (tipo == 'i')
        {
            printf("Valor de ax: %d\n", reg->i32);
            reg->i32 -= atoi(segundoParametro);
        }
        else if (tipo == 'u')
        {
            reg->u8 -= atoi(segundoParametro);
        }
        break;
    case JNZ:
        reg = obtenerRegistro(primerParametro, procesoCPU, &tipo);
        instruccion_JNZ(procesoCPU, reg, tipo, atoi(segundoParametro));
        break;
    case IO_GEN_SLEEP_CPU:
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_IO, instruccion, CPUsocketBidireccionalDispatch);
        break;
    case EXIT:
        enviar_contexto_al_kernel(procesoCPU, EXIT_SIGNAL, NULL, CPUsocketBidireccionalDispatch);
        log_destroy(loger_execute);
        break;
    default:
        break;
    }
}

Registro *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU, char *tipo)
{
    if (strcmp(registro, "ax") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.ax);
    }
    else if (strcmp(registro, "eax") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.eax);
    }
    else if (strcmp(registro, "bx") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.bx);
    }
    else if (strcmp(registro, "ebx") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.ebx);
    }
    else if (strcmp(registro, "cx") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.cx);
    }
    else if (strcmp(registro, "ecx") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.ecx);
    }
    else if (strcmp(registro, "dx") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.dx);
    }
    else if (strcmp(registro, "edx") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.edx);
    }
    else
    {
        return NULL;
    }
}

void checkInterrupt(Contexto_proceso *procesoCPU, int *interrupcion, int *CPUsocketBidireccionalDispatch)
{
    if (procesoCPU->pid == -1)
        return;

    if (*interrupcion == 1)
    {
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_KERNEL, NULL, CPUsocketBidireccionalDispatch);
        *interrupcion = 0;
        procesoCPU->pid = -1;
    }
    else if (*interrupcion == 2)
    {
        enviar_contexto_al_kernel(procesoCPU, FIN_DE_QUANNTUM, NULL, CPUsocketBidireccionalDispatch);
        *interrupcion = 0;
        procesoCPU->pid = -1;
    }
}

void instruccion_JNZ(Contexto_proceso *procesoCPU, Registro *reg, char tipo, int valor)
{

    if (tipo == 'i')
    {
        if (reg->i32 != 0)
            procesoCPU->pc = valor;
    }
    else if (tipo == 'u')
    {
        if (reg->u8 != 0)
            procesoCPU->pc = valor;
    }
}

void enviar_contexto_al_kernel(Contexto_proceso *procesoCPU, MotivoDesalojo motivo, char *instruccion, int *CPUsocketBidireccionalDispatch)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t) * 6 + strlen(instruccion) + 1 + sizeof(MotivoDesalojo) + sizeof(uint8_t) * 4);
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
    char **instruccionSeparada = string_n_split(instruccion, 2, " ");
    string_append(&result, instruccionSeparada[0]);
    string_append(&result, " - ");
    string_append(&result, instruccionSeparada[1]);

    return result;
}