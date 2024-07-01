#include "enviarMensaje.h"

int enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion)
{
    void *a_enviar = malloc(buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;
    int bytesEnviados;

    memcpy(a_enviar + offset, &(modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(codigoOperacion), sizeof(op_code));

    if (codigoOperacion != HANDSHAKE)
    {
        offset += sizeof(op_code);
        memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(a_enviar + offset, buffer->stream, buffer->size);
        bytesEnviados = send(*socket, a_enviar, buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t), 0);
    }
    else
        bytesEnviados = send(*socket, a_enviar, sizeof(TipoModulo) + sizeof(op_code), 0);

    buffer_destroy(buffer);
    free(a_enviar);

    if (bytesEnviados == -1) // manejo de error si no se puede enviar nada
        return -1;

    return bytesEnviados;
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