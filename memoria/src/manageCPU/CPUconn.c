#include "CPUconn.h"

void *manage_conn_cpu(void *ptr)
{
    int socketCPU = *((int *)ptr);

    while (1)
    {
        op_code *op_code = get_opcode_msg_recv(socketCPU);
        TipoModulo *modulo = get_modulo_msg_recv(socketCPU);
        void *stream = buffer_leer_stream_recv(socketCPU);

        enviar_instruccion(socketCPU, obtener_pc(stream), obtener_pid(stream));
    }
}

unsigned int obtener_pid(void *stream)
{
    unsigned int pid;
    memcpy(&pid, stream, sizeof(unsigned int));
    return pid;
}

unsigned int obtener_pc(void *stream)
{
    unsigned int pc;
    memcpy(&pc, stream + sizeof(unsigned int), sizeof(unsigned int));
    return pc;
}

void enviar_instruccion(int *socket, unsigned int pc, unsigned int pid)
{
    char *instruccion = obtener_instruccion(pid, pc);
    // implementar
}