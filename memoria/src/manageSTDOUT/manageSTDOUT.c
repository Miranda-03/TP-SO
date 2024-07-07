#include "manageSTDOUT.h"

void *manage_conn_stdout_io(void *ptr)
{
    int socketSTDOUT = *((int *)ptr);

    int pid;
    int dir_fisica;
    int bytes;
    // HAY QUE PONER LOS LOGERS

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(&socketSTDOUT);
        op_code *op_code = get_opcode_msg_recv(&socketSTDOUT);
        t_buffer *buffer = buffer_leer_recv(&socketSTDOUT);
        pid = buffer_read_uint32(buffer);

        switch (*op_code)
        {
        case LEER_MEMORIA:
            dir_fisica = buffer_read_uint32(buffer);
            bytes = buffer_read_uint32(buffer);
            buffer_destroy(buffer);
            leer_memoria(dir_fisica, bytes, &socketSTDOUT);
            break;
        }
    }
}