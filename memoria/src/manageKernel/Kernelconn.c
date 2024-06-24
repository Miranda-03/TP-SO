#include "Kernelconn.h"

void *manage_conn_kernel(void *ptr)
{
    int *socketKernel = ((int *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(socketKernel);
        op_code *op_code = get_opcode_msg_recv(socketKernel);
        t_buffer *buffer = buffer_leer_recv(socketKernel);
        printf("le llega algo del kernel\n");
        if (buffer_read_uint32(buffer) == 1)
        {
            printf("entra al if para guardar la instruccion \n");
            unsigned int pid = buffer_read_uint32(buffer);
            char *path = obtener_path_instruccion(buffer);
            enviar_mensaje(socketKernel, agregar_instrucciones(path, pid));
            printf("guarda la instruccion \n");
        }
        else
        {
            // quitar archivo de instrucciones
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

void enviar_mensaje(int *socket, int instruccion_guardada)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, instruccion_guardada);
    enviarMensaje(socket, buffer, MEMORIA, MENSAJE);
}