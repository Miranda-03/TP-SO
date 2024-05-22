

void * manageDISPATCH(void * ptr){

    int socket = * ((int * )ptr);

    while(1){
        op_code * op_code = get_opcode_msg_recv(socket);
        TipoModulo * modulo = get_modulo_msg_recv(socket);
        // agarro el PCB
    }
}

void * manageINTERRUPT(void * ptr){

    int socket = * ((int * )ptr);

    while(1){
        op_code * op_code = get_opcode_msg_recv(socket);
        TipoModulo * modulo = get_modulo_msg_recv(socket);
    }
}