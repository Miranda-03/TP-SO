#include "connKernel.h"

void * manageDISPATCH(void * ptr){

    parametros_hilo *params = * ((parametros_hilo * )ptr)

    while(1){
        op_code * op_code = get_opcode_msg_recv(parametros_hilo->socket);
        TipoModulo * modulo = get_modulo_msg_recv(parametros_hilo->socket);
        
        void *stream = buffer_leer_stream_recv(parametros_hilo->socket);

        memcpy(parametros_hilo->procesoCPU->)
    }
}

void * manageINTERRUPT(void * ptr){

    parametros_hilo *params = * ((parametros_hilo * )ptr)

    while(1){
        op_code * op_code = get_opcode_msg_recv(parametros_hilo->socket);
        TipoModulo * modulo = get_modulo_msg_recv(parametros_hilo->socket);
    }
}