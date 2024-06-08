#include "connMemoria.h"

char *recibirInstruccion(int *socket, unsigned int pid, unsigned int pc)
{

    t_buffer *buffer = buffer_create(sizeof(unsigned int) * 2);

    buffer_add_uint32(buffer, pc);
    buffer_add_uint32(buffer, pid);

    enviarMensaje(socket, buffer, CPU, MENSAJE);

    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *op_code = get_opcode_msg_recv(socket);
    t_buffer *buffer_recv = buffer_leer_recv(socket);

    int size = buffer_read_uint32(buffer_recv);
    char *instruccion = buffer_read_string(buffer, size);

    buffer_destroy(buffer);
    buffer_destroy(buffer_recv);

    return instruccion;
}