#include "IOconexion.h"
/* void manageIO(void* ptr)
{
    parametros_hilo_IO* params = ((parametros_hilo_IO *)ptr);
    op_code *opCode = get_opcode_msg_recv(params->socket);
    if (!opCode) {
        perror("get_opcode_msg_recv");
        return;
    }
//impresora
t_buffer *buffer = buffer_leer_recv(params->socket);
    params->interfaz = buffer_read_uint32(buffer);
    int sizeIdentificador = buffer_read_uint32(buffer);
    params->identificador = buffer_read_string(buffer, sizeIdentificador);
    buffer_destroy(buffer);
if (*opCode == HANDSHAKE)
    {
        guardar_interfaz_conectada(socket, params->interfaz, params->identificador, interfaces_conectadas);
        enviarPaqueteResult(1, socket, KERNEL, IO);
    }
else if(*opCode==MENSAJE)
{

}
    else
    {
       enviarPaqueteResult(-1, socket, KERNEL, IO);
    }
} */