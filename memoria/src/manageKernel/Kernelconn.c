#include "Kernelconn.h"

void *manage_conn_kernel(void *ptr)
{
    int *socketKernel = ((int *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(socketKernel);
        op_code *op_code = get_opcode_msg_recv(socketKernel);
        t_buffer *buffer = buffer_leer_recv(socketKernel);

        if (buffer_read_uint32(buffer) == 1)
        {
            unsigned int pid = buffer_read_uint32(buffer);
            char *path = obtener_path_instruccion(buffer);

            enviar_mensaje_de_confirmacion(socketKernel, guardar_nuevo_proceso(path, pid));
        }
        else
        {
            int pid = buffer_read_uint32(buffer);

            resize_proceso(pid, 0, 0);
            quitar_tabla_de_pagina(pid);
            int respuesta = quitar_instrucciones(pid);

            t_buffer *buffer_eliminar_instrucciones = buffer_create(4);
            buffer_add_uint32(buffer_eliminar_instrucciones, respuesta);
            enviarMensaje(socketKernel, buffer_eliminar_instrucciones, MEMORIA, MENSAJE);
        }
        buffer_destroy(buffer);
        free(modulo);
        free(op_code);
    }
}

char *obtener_path_instruccion(t_buffer *buffer)
{
    int path_len = buffer_read_uint32(buffer);
    char *path = buffer_read_string(buffer, path_len);
    return path;
}

unsigned int obtener_instuccion_kernel(void *stream)
{
    unsigned int instruccion;
    memcpy(&instruccion, stream, sizeof(unsigned int));
    return instruccion;
}

void enviar_mensaje_de_confirmacion(int *socket, int instruccion_guardada)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, instruccion_guardada);
    enviarMensaje(socket, buffer, MEMORIA, MENSAJE);
}

int guardar_nuevo_proceso(char *path, int pid)
{
    int resultado_instrucciones = agregar_instrucciones(path, pid);

    if(resultado_instrucciones > 0)
    {
        crear_tabla_de_pagina(pid);
        return 1;
    }
    
    return -1;
}