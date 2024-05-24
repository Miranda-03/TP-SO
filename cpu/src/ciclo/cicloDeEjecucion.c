#include "cicloDeEjecucion.h"

void cicloDeEjecucion(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion)
{
    while (1)
    {

        if (procesoCPU->pid != NULL)
        {

            // FETCH
            char *instruccion = recibirInstruccion(socketMemoria, procesoCPU->pid, procesoCPU->registros.pc);

            procesoCPU->registros.pc += 1;

            // DECODE
            char *instruccionSeparada[] = string_split(instruccion, " ");

            execute(instruccionSeparada, procesoCPU, instruccion, CPUsocketBidireccionalDispatch);

            checkInterrupt(procesoCPU, interrupcion);
        }
    }
}

void execute(char *instruccionSeparada[], Contexto_proceso *procesoCPU, char *instruccion, int *CPUsocketBidireccionalDispatch)
{

    char operacion = instruccionSeparada[0];
    char *primerParametro = instruccionSeparada[1];
    char *segundoParametro = instruccionSeparada[2];

    switch (operacion)
    {
    case SET:
        int *registro = obtenerRegistro(primerParametro, procesoCPU);
        *registro = atoi(&segundoParametro);
        break;
    case SUM:
        int *registro = obtenerRegistro(primerParametro, procesoCPU);
        *registro += atoi(&segundoParametro);
        break;
    case SUB:
        int *registro = obtenerRegistro(primerParametro, procesoCPU);
        *registro -= atoi(&segundoParametro);
        break;
    case JNZ:
        instruccion_JNZ(procesoCPU, obtenerRegistro(primerParametro, procesoCPU), atoi(&segundoParametro));
        break;
    case IO_GEN_SLEEP:
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_IO, instruccion, CPUsocketBidireccionalDispatch) break;
    case EXIT:
        enviar_contexto_al_kernel(procesoCPU, EXIT_SIGNAL, NULL, CPUsocketBidireccionalDispatch);
        break;
    default:
        break;
    }
}

int *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU)
{
    switch (registro)
    {
    case ax:
        return &(procesoCPU->registros.ax);
        break;
    case eax:
        return &(procesoCPU->registros.eax);
        break;
    case bx:
        return &(procesoCPU->registros.bx);
        break;
    case ebx:
        return &(procesoCPU->registros.ebx);
        break;
    case cx:
        return &(procesoCPU->registros.cx);
        break;
    case ecx:
        return &(procesoCPU->registros.ecx);
        break;
    case dx:
        return &(procesoCPU->registros.dx);
        break;
    case edx:
        return &(procesoCPU->registros.edx);
        break;
    default:
        break;
    }
}

void checkInterrupt(Contexto_proceso *procesoCPU, int *interrupcion, int *CPUsocketBidireccionalDispatch)
{
    if (procesoCPU->pid == NULL)
        return;

    if (*interrupcion == 1)
    {
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_KERNEL, NULL, CPUsocketBidireccionalDispatch);
        *interrupcion = 0;
        procesoCPU->pid = NULL;
    }
    else if (*interrupcion == 2)
    {
        enviar_contexto_al_kernel(procesoCPU, FIN_DE_QUANNTUM, NULL, CPUsocketBidireccionalDispatch);
        *interrupcion = 0;
        procesoCPU->pid = NULL;
    }
}

void instruccion_JNZ(Contexto_proceso *procesoCPU, int *registro, int valor)
{
    if (*registro != 0)
    {
        procesoCPU->registros.pc = valor;
    }
}

void enviar_contexto_al_kernel(Contexto_proceso *procesoCPU, MotivoDesalojo motivo, char *instruccion, int *CPUsocketBidireccionalDispatch)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t) * 6 + strlen(instruccion) + 1 + sizeof(MotivoDesalojo) + sizeof(uint8_t) * 4);
    buffer_add_uint32(buffer, motivo); // No estoy seguto si era un uint32_t
    buffer_add_uint32(buffer, procesoCPU->pid);
    agregar_registros_al_buffer(procesoCPU, buffer);

    if (instruccion != NULL)
    { // No estoy seguro de la comparacion
        buffer_add_uint32(buffer, strlen(instruccion) + 1);
        buffer_add_string(buffer, instruccion);
    }

    enviarMensaje(CPUsocketBidireccionalDispatch, buffer, CPU, MENSAJE);

    procesoCPU->pid = NULL;
}

void agregar_registros_al_buffer(Contexto_proceso *procesoCPU, t_buffer *buffer)
{
    buffer_add_uint32(buffer, procesoCPU->registros.pc);
    buffer_add_uint8(buffer, procesoCPU->registros.ax);
    buffer_add_uint32(buffer, procesoCPU->registros.eax);
    buffer_add_uint8(buffer, procesoCPU->registros.bx);
    buffer_add_uint32(buffer, procesoCPU->registros.ebx);
    buffer_add_uint8(buffer, procesoCPU->registros.cx);
    buffer_add_uint32(buffer, procesoCPU->registros.ecx);
    buffer_add_uint8(buffer, procesoCPU->registros.dx);
    buffer_add_uint32(buffer, procesoCPU->registros.edx);
}