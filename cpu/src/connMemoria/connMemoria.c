#include "connMemoria.h"


char* recibirInstruccion(int *socket, unsigned int pid, unsigned int pc){

    int* size = malloc(sizeof(int));
    

    t_buffer* buffer= buffer_create(sizeof(unsigned int)*2);

    buffer_add_uint32(buffer, pc);
    buffer_add_uint32(buffer, pid);
    
    enviarMensaje(socket, buffer, CPU, MENSAJE);

    op_code * op_code = get_opcode_msg_recv(socket);
    TipoModulo *   modulo = get_modulo_msg_recv(socket);

    void* stream = buffer_leer_stream_recv(socket);

    memcpy(size, stream, sizeof(int));
    char* instruccion = malloc(*size);
    memcpy(instruccion, stream + sizeof(int), *size);

    buffer_destroy(buffer);
    free(size);
    
    return instruccion;
}