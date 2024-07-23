#include "manageDIALFS.h"

void *manage_conn_dialFS_io(void *ptr)
{
    int socketDIALFS = *((int *)ptr);

    int pid;
    int dir_fisica;
    int bytes;
    // HAY QUE PONER LOS LOGERS
    int conectada = 1;

    while (conectada > 0)
    {
        TipoModulo *modulo = get_modulo_msg_recv(&socketDIALFS);
        op_code *op_code = get_opcode_msg_recv(&socketDIALFS);
        t_buffer *buffer = buffer_leer_recv(&socketDIALFS);
        pid = buffer_read_uint32(buffer);

        switch (*op_code)
        {
        case ESCRIBIR_MEMORIA:
            dir_fisica = buffer_read_uint32(buffer);
            bytes = buffer_read_uint32(buffer);
            void *dato = malloc(bytes);
            buffer_read(buffer, dato, bytes);
            buffer_destroy(buffer);
            char *dato_char = (char *)dato;
            escribir_memoria(pid,dir_fisica, bytes, dato, &socketDIALFS);
            break;

        case LEER_MEMORIA:
            dir_fisica = buffer_read_uint32(buffer);
            bytes = buffer_read_uint32(buffer);
            buffer_destroy(buffer);
            leer_memoria(pid,dir_fisica, bytes, &socketDIALFS);
            break;

        default:
            buffer_destroy(buffer);
            conectada = -1;
            break;
        }
    }
}
