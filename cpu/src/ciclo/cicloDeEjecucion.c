#include "cicloDeEjecucion.h"

void cicloDeEjecucion(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion)
{
    while (1)
    {

        if (procesoCPU->pid != NULL)
        {

            // FETCH
            char *instruccion = recibirInstruccion(CPUSocketMemoria, procesoCPU->pid, procesoCPU->registros.pc);

            procesoCPU->registros.pc += 1;

            // DECODE
          
            char** instruccionSeparada = string_split(instruccion, " ");

            execute(instruccionSeparada, procesoCPU, instruccion, CPUsocketBidireccionalDispatch);

            checkInterrupt(procesoCPU, interrupcion, CPUsocketBidireccionalDispatch);
        }
    }
}

void execute(char instruccionSeparada[], Contexto_proceso *procesoCPU, char *instruccion, int *CPUsocketBidireccionalDispatch)
{

    char *operacion = instruccionSeparada[0];
    char *primerParametro = instruccionSeparada[1];
    char *segundoParametro = instruccionSeparada[2];
    int *registro;

    switch (*operacion)
    {
    case SET:
        registro = obtenerRegistro(primerParametro, procesoCPU);
        *registro = atoi(&segundoParametro);
        break;
    case SUM:
        registro = obtenerRegistro(primerParametro, procesoCPU);
        *registro += atoi(&segundoParametro);
        break;
    case SUB:
        registro = obtenerRegistro(primerParametro, procesoCPU);
        *registro -= atoi(&segundoParametro);
        break;
    case JNZ:
        instruccion_JNZ(procesoCPU, obtenerRegistro(primerParametro, procesoCPU), atoi(&segundoParametro));
        break;
    case IO_GEN_SLEEP:
        enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_IO, instruccion, CPUsocketBidireccionalDispatch); 
        break;
    case EXIT:
        enviar_contexto_al_kernel(procesoCPU, EXIT_SIGNAL, NULL, CPUsocketBidireccionalDispatch);
        break;
    default:
        break;
    }
}

int *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU)
{
    if (strcmp(registro, "ax") == 0)
    {
        return &(procesoCPU->registros.ax);
    }
    else if (strcmp(registro, "eax") == 0)
    {
        return &(procesoCPU->registros.eax);
    }
    else if (strcmp(registro, "bx") == 0)
    {
        return &(procesoCPU->registros.bx);
    }
    else if (strcmp(registro, "ebx") == 0)
    {
        return &(procesoCPU->registros.ebx);
    }
    else if (strcmp(registro, "cx") == 0)
    {
        return &(procesoCPU->registros.cx);
    }
    else if (strcmp(registro, "ecx") == 0)
    {
        return &(procesoCPU->registros.ecx);
    }
    else if (strcmp(registro, "dx") == 0)
    {
        return &(procesoCPU->registros.dx);
    }
    else if (strcmp(registro, "edx") == 0)
    {
        return &(procesoCPU->registros.edx);
    }
    else
    {
        // Devolver un valor por defecto en caso de que no coincida ningún registro
        return NULL;
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
        buffer_add_string(buffer, strlen(instruccion) + 1, instruccion);
    }

    enviarMensaje(CPUsocketBidireccionalDispatch, buffer, CPU, MENSAJE);
    buffer_destroy(buffer);
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