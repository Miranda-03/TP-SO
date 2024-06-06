#include "connKernel.h"

 void *manageDISPATCH(void *ptr)
{

    parametros_hilo_Cpu *params = ((parametros_hilo_Cpu *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);

        obtener_procesoCPU_del_stream(buffer_leer_recv(params->socket), params->procesoCPU);
    }
}

void *manageINTERRUPT(void *ptr)
{

    parametros_hilo_Cpu *params = ((parametros_hilo_Kernel *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);
        t_buffer *buffer = buffer_leer_recv(socket);

        params->interrupcion = buffer_read_uint32(buffer);// pasar un 1 como unsigned int para interrumpir, un 2 para indicar fin de quanntum

        buffer_destroy(buffer);
    }
}

void obtener_procesoCPU_del_stream(t_buffer *buffer, Contexto_proceso *procesoCPU)
{
    
    procesoCPU->pid = buffer_read_uint32(buffer);
    obtener_registros(buffer, procesoCPU);

    buffer_destroy(buffer);
}

void obtener_registros(t_buffer *buffer, Contexto_proceso *procesoCPU)

{
    procesoCPU->registros.pc = buffer_read_uint32(buffer);
    procesoCPU->registros.ax = buffer_read_uint8(buffer);
    procesoCPU->registros.eax = buffer_read_uint32(buffer);
    procesoCPU->registros.bx = buffer_read_uint8(buffer);
    procesoCPU->registros.ebx = buffer_read_uint32(buffer);
    procesoCPU->registros.cx = buffer_read_uint8(buffer);
    procesoCPU->registros.ecx = buffer_read_uint32(buffer);
    procesoCPU->registros.dx = buffer_read_uint8(buffer);
    procesoCPU->registros.edx = buffer_read_uint32(buffer);
}

