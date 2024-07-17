#include "manageSTDIN.h"

void *manage_conn_stdin_io(void *ptr)
{
    int socketSTDIN = *((int *)ptr);

    int pid;
    int dir_fisica;
    int bytes;
    // HAY QUE PONER LOS LOGERS

    int conectada = 1;

    printf("ENTRA EN EL HILO DE STDIN\n");

    while (conectada > 0)
    {
        TipoModulo *modulo = get_modulo_msg_recv(&socketSTDIN);
        op_code *op_code = get_opcode_msg_recv(&socketSTDIN);
        t_buffer *buffer = buffer_leer_recv(&socketSTDIN);
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
            escribir_memoria(pid,dir_fisica, bytes, dato, &socketSTDIN);
            break;

        default:
            buffer_destroy(buffer);
            conectada = -1;
            break;
        }
    }
}