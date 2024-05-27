#include "CPUconn.h"

void *manage_conn_cpu(void *ptr)
{
    int socketCPU = ((int *)ptr);

    unsigned int pc;
    unsigned int pid;

    while (1)
    {
        op_code *op_code = get_opcode_msg_recv(socketCPU);
        TipoModulo *modulo = get_modulo_msg_recv(socketCPU);
        //void *stream = buffer_leer_stream_recv(socketCPU);

        t_buffer *buffer = buffer_leer_recv(socket);
        pc = buffer_read_uint32(buffer);
        pid = buffer_read_uint32(buffer);
        buffer_destroy(buffer);
        enviar_instruccion(socketCPU, pc, pid);
    }
}

void enviar_instruccion(int *socket, unsigned int pc, unsigned int pid)
{
    char *instruccion = obtener_instruccion(pid, pc);
    t_buffer *buffer = buffer_create(sizeof(uint32_t) + strlen(instruccion) + 1);
    buffer_add_uint32(buffer,  strlen(instruccion) + 1);
    buffer_add_string(buffer, strlen(instruccion) + 1, instruccion);
    enviarMensaje(socket, buffer, MEMORIA, MENSAJE);
}