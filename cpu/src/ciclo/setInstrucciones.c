#include "setInstrucciones.h"

void instruccion_SUM(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    if (strcmp(primerParametro, "PC") == 0)
    {
        procesoCPU->pc += atoi(segundoParametro);
        return;
    }
    else if (strcmp(primerParametro, "SI") == 0)
    {
        procesoCPU->SI += atoi(segundoParametro);
    }
    else if (strcmp(primerParametro, "DI") == 0)
    {
        procesoCPU->DI += atoi(segundoParametro);
    }

    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    Registro *reg2 = obtenerRegistro(segundoParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 += reg2->i32;
    }
    else if (*tipo == 'u')
    {
        reg->u8 += reg2->u8;
    }
}

void instruccion_SET(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    if (strcmp(primerParametro, "PC") == 0)
    {
        procesoCPU->pc += atoi(segundoParametro);
        return;
    }
    else if (strcmp(primerParametro, "SI") == 0)
    {
        procesoCPU->SI += atoi(segundoParametro);
    }
    else if (strcmp(primerParametro, "DI") == 0)
    {
        procesoCPU->DI += atoi(segundoParametro);
    }

    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 = atoi(segundoParametro);
    }
    else if (*tipo == 'u')
    {
        reg->u8 = atoi(segundoParametro);
    }
}

void instruccion_JNZ(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    int valor = atoi(segundoParametro);
    if (*tipo == 'i')
    {
        if (reg->i32 != 0)
            procesoCPU->pc = valor;
    }
    else if (*tipo == 'u')
    {
        if (reg->u8 != 0)
            procesoCPU->pc = valor;
    }
}

void instruccion_SUB(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    if (strcmp(primerParametro, "PC") == 0)
    {
        procesoCPU->pc += atoi(segundoParametro);
        return;
    }
    else if (strcmp(primerParametro, "SI") == 0)
    {
        procesoCPU->SI += atoi(segundoParametro);
    }
    else if (strcmp(primerParametro, "DI") == 0)
    {
        procesoCPU->DI += atoi(segundoParametro);
    }

    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    Registro *reg2 = obtenerRegistro(segundoParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 -= reg2->i32;
    }
    else if (*tipo == 'u')
    {
        reg->u8 -= reg2->u8;
    }
}

void instruccion_RESIZE(char *primerParametro, Contexto_proceso *procesoCPU, int socket, int *socketDispatch)
{
    t_buffer *buffer = buffer_create(8);
    buffer_add_uint32(buffer, procesoCPU->pid);
    buffer_add_uint32(buffer, atoi(primerParametro));
    enviarMensaje(&socket, buffer, CPU, RESIZE);

    int *resultado = malloc(4);
    TipoModulo *modulo = get_modulo_msg_recv(&socket);
    op_code *opcode = get_opcode_msg_recv(&socket);
    t_buffer *buffer_recv = buffer_leer_recv(&socket);

    buffer_read(buffer_recv, resultado, 4);

    if (*resultado < 0)
        enviar_contexto_al_kernel(procesoCPU, OUT_OF_MEMORY, NULL, socketDispatch);

    free(resultado);
    buffer_destroy(buffer_recv);
}

void instruccion_COPY_STRING(char *primerParametro, Contexto_proceso *procesoCPU, int socket_memoria)
{
    char *string_obtenido = (char *)cpu_leer_memoria(procesoCPU->SI, atoi(primerParametro), procesoCPU->pid, socket_memoria);
    escribir_memoria(procesoCPU->DI, atoi(primerParametro), procesoCPU->pid, string_obtenido, socket_memoria);
}

void instruccion_MOV_IN(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg, int socket_memoria)
{
    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    Registro *reg2 = obtenerRegistro(segundoParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 = *((uint32_t *)cpu_leer_memoria(reg2->i32, 4, procesoCPU->pid, socket_memoria));
    }
    else if (*tipo == 'u')
    {
        reg->u8 = *((uint8_t *)cpu_leer_memoria(reg2->u8, 1, procesoCPU->pid, socket_memoria));
    }
}

void instruccion_MOV_OUT(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg, int socket_memoria)
{
    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    Registro *reg2 = obtenerRegistro(segundoParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        escribir_memoria(reg->i32, 4, procesoCPU->pid, &(reg2->i32), socket_memoria);
    }
    else if (*tipo == 'u')
    {
        escribir_memoria(reg->u8, 1, procesoCPU->pid, &(reg2->u8), socket_memoria);
    }
}

void instruccion_IO_STD(char **instruccion, Contexto_proceso *procesoCPU, char *tipo, Registro *reg, int socket_memoria, int socket_dispatch, TipoInterfaz interfaz)
{
    unsigned int direccion = 0;
    unsigned int tam = 0;

    reg = obtenerRegistro(instruccion[2], procesoCPU, tipo);
    if (*tipo == 'i')
    {
        direccion = reg->i32;
    }
    else if (*tipo == 'u')
    {
        direccion = reg->u8;
    }

    Registro *reg2 = obtenerRegistro(instruccion[3], procesoCPU, tipo);
    if (*tipo == 'i')
    {
        tam = reg2->i32;
    }
    else if (*tipo == 'u')
    {
        tam = reg2->u8;
    }

    char **direcciones_fisicas;

    if (interfaz == STDIN)
        direcciones_fisicas = obtener_direcciones_fisicas(direccion, tam, procesoCPU->pid);
    else
        direcciones_fisicas = obtener_direcciones_fisicas(tam, direccion, procesoCPU->pid);

    char *instruccion_con_dir_fisica = string_new();

    string_append(&instruccion_con_dir_fisica, instruccion[0]);
    string_append(&instruccion_con_dir_fisica, " ");
    string_append(&instruccion_con_dir_fisica, instruccion[1]);
    string_append(&instruccion_con_dir_fisica, " ");
    string_append(&instruccion_con_dir_fisica, "[");

    int len_df_array = string_array_size(direcciones_fisicas);

    for (int i = 0; i < (len_df_array - 1); i++)
    {
        string_append(&instruccion_con_dir_fisica, direcciones_fisicas[i]);
        string_append(&instruccion_con_dir_fisica, ",");
    }

    string_append(&instruccion_con_dir_fisica, direcciones_fisicas[len_df_array - 1]);
    string_append(&instruccion_con_dir_fisica, "]");

    enviar_contexto_al_kernel(procesoCPU, INTERRUPCION_IO, instruccion_con_dir_fisica, &socket_dispatch);
}

Registro *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU, char *tipo)
{
    if (strcmp(registro, "AX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.ax);
    }
    else if (strcmp(registro, "EAX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.eax);
    }
    else if (strcmp(registro, "BX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.bx);
    }
    else if (strcmp(registro, "EBX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.ebx);
    }
    else if (strcmp(registro, "CX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.cx);
    }
    else if (strcmp(registro, "ECX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.ecx);
    }
    else if (strcmp(registro, "DX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.dx);
    }
    else if (strcmp(registro, "EDX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.edx);
    }
    else
    {
        return NULL;
    }
}