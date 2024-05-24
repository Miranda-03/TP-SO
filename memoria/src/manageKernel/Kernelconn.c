#include "Kernelconn.h"

void *manage_conn_kernel(void *ptr)
{
    int socketKernel = *((int *)ptr);

    while (1)
    {
        op_code *op_code = get_opcode_msg_recv(socketKernel);
        TipoModulo *modulo = get_modulo_msg_recv(socketKernel);
        void *stream = buffer_leer_stream_recv(socketKernel);

        if (obtener_instuccion_kernel(stream) == 1)
        {
            enviar_mensaje(socketKernel, agregar_instrucciones(obtener_path_instruccion(stream), obtener_pid(stream)));
        }
        else
        {
            // quitar archivo de instrucciones
        }
    }
}

unsigned int obtener_elpid(void *stream)
{
    unsigned int pid;
    memcpy(&pid, stream + sizeof(unsigned int), sizeof(unsigned int));
    return pid;
}

char *obtener_path_instruccion(void *stream)
{
    char *path;
    uint32_t path_len;
    memcpy(&path_len, stream + sizeof(unsigned int), sizeof(uint32_t));
    memcpy(path, stream + (sizeof(unsigned int) * 2) + sizeof(uint32_t), path_len);
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