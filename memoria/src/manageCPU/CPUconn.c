#include "CPUconn.h"

void *manage_conn_cpu(void *ptr)
{
    int socketCPU = *((int *)ptr);

    obtener_socket(&socketCPU);

    int pc;
    int pid;
    int dir_fisica;
    int bytes;
    int tam;
    int numero_pagina;
    // HAY QUE PONER LOS LOGERS

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(&socketCPU);
        op_code *op_code = get_opcode_msg_recv(&socketCPU);
        t_buffer *buffer = buffer_leer_recv(&socketCPU);
        pid = buffer_read_uint32(buffer);

        switch (*op_code)
        {
        case OBTENER_INSTRUCCION:
            pc = buffer_read_uint32(buffer);
            buffer_destroy(buffer);
            enviar_instruccion(&socketCPU, pc, pid);
            break;

        case LEER_MEMORIA:
            dir_fisica = buffer_read_uint32(buffer);
            bytes = buffer_read_uint32(buffer);
            buffer_destroy(buffer);
            leer_memoria(dir_fisica, bytes, NULL);
            break;
        
        case ESCRIBIR_MEMORIA:
            dir_fisica = buffer_read_uint32(buffer);
            bytes = buffer_read_uint32(buffer);
            void *dato = malloc(bytes);
            buffer_read(buffer, dato, bytes);
            buffer_destroy(buffer);
            escribir_memoria(dir_fisica, bytes, dato, NULL);
            break;
        
        case RESIZE:
            tam = buffer_read_uint32(buffer);
            buffer_destroy(buffer);
            resize_proceso(pid, tam);
            break;
        
        case OBTENER_MARCO:
            numero_pagina = buffer_read_uint32(buffer);
            buffer_destroy(buffer);
            encontrar_marco(pid, numero_pagina);
            break;

        default:
            buffer_destroy(buffer);
            enviar_tam_de_pagina();
            break;
        }
    }
}