#include "enviarMensaje.h"

void enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion){
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->modulo = modulo;
    paquete->opcode = codigoOperacion;
    paquete->buffer = buffer;

    void *a_enviar = malloc(buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(paquete->opcode), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, &(paquete->buffer->stream), paquete->buffer->size);

    send(socket, a_enviar, buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t));

    buffer_destroy(paquete->buffer);
    free(a_enviar);
    free(paquete);
}

op_code *get_opcode_msg_recv(int *socket){
    op_code *opcode = malloc(sizeof(op_code));
    recv(socket, opcode, sizeof(op_code), 0);
    return opcode;
}