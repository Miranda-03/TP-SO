#include "enviarMensaje.h"

void enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion)
{
    void *a_enviar = malloc(buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(codigoOperacion), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, buffer->stream, buffer->size);
    send(*socket, a_enviar, buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t), 0);

    buffer_destroy(buffer);
    free(a_enviar);
}

op_code *get_opcode_msg_recv(int *socket)
{
    op_code *opcode = malloc(sizeof(op_code));
    recv(*socket, opcode, sizeof(op_code), MSG_WAITALL);
    return opcode;
}

TipoModulo *get_modulo_msg_recv(int *socket)
{
    TipoModulo *modulo = malloc(sizeof(TipoModulo));
    recv(*socket, modulo, sizeof(TipoModulo), MSG_WAITALL);
    return modulo;
}