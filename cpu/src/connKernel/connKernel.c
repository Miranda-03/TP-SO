#include "connKernel.h"

void *manageDISPATCH(void *ptr)
{

    parametros_hilo *params = *((parametros_hilo *)ptr);

    while (1)
    {
        op_code *op_code = get_opcode_msg_recv(params->socket);
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);

        obtener_procesoCPU_del_stream(buffer_leer_stream_recv(params->socket), params->procesoCPU);
    }
}

void *manageINTERRUPT(void *ptr)
{

    parametros_hilo *params = *((parametros_hilo *)ptr);

    while (1)
    {
        op_code *op_code = get_opcode_msg_recv(params->socket);
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        void *stream = buffer_leer_stream_recv(params->socket);
        memcpy(params->interrupcion, stream, sizeof(unsigned int)); // pasar un 1 como unsigned int para interrumpir
                                                                    // un 2 para indicar fin de quanntum
    }
}

void obtener_procesoCPU_del_stream(void *stream, Contexto_proceso *procesoCPU)
{

    unsigned int *offset = malloc(sizeof(unsigned int));
    *offset = 0;

    memcpy(procesoCPU->pcb.pid, stream + *offset, sizeof(unsigned int));
    *offset += sizeof(unsigned int);
    obtener_registros(stream, offset, procesoCPU);

    free(stream);
    free(offset);
}

void obtener_registros(void *stream, unsigned int *offset, Contexto_proceso *procesoCPU)
{
    memcpy(procesoCPU->registros.pc, stream + *offset, sizeof(uint32_t));
    *offset += sizeof(uint32_t);
    memcpy(procesoCPU->registros.ax, stream + *offset, sizeof(uint8_t));
    *offset += sizeof(uint8_t);
    memcpy(procesoCPU->registros.eax, stream + *offset, sizeof(uint32_t));
    *offset += sizeof(uint32_t);
    memcpy(procesoCPU->registros.bx, stream + *offset, sizeof(uint8_t));
    *offset += sizeof(uint8_t);
    memcpy(procesoCPU->registros.ebx, stream + *offset, sizeof(uint32_t));
    *offset += sizeof(uint32_t);
    memcpy(procesoCPU->registros.cx, stream + *offset, sizeof(uint8_t));
    *offset += sizeof(uint8_t);
    memcpy(procesoCPU->registros.ecx, stream + *offset, sizeof(uint32_t));
    *offset += sizeof(uint32_t);
    memcpy(procesoCPU->registros.dx, stream + *offset, sizeof(uint8_t));
    *offset += sizeof(uint8_t);
    memcpy(procesoCPU->registros.edx, stream + *offset, sizeof(uint32_t));
}