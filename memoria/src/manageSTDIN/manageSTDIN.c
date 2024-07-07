#include "manageSTDIN.h"

void *manage_conn_stdin_io(void *ptr)
{
    int socketSTDIN = *((int *)ptr);

    int pid;
    int dir_fisica;
    int bytes;
    // HAY QUE PONER LOS LOGERS

    printf("ENTRA EN EL HILO DE STDIN\n");

    while (1)
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

            char destino2[2];
            memset(destino2, 0, 2);

            memcpy(destino2, dato, 2);

            printf("%s\n", destino2);

            buffer_destroy(buffer);
            escribir_memoria(dir_fisica, bytes, dato, &socketSTDIN);
            break;
        }
    }
}