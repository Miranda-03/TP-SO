#include "connKernel.h"

void *manageDISPATCH(void *ptr)
{

    parametros_hilo_Cpu *params = ((parametros_hilo_Cpu *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);

        if (*op_code == OBTENER_IP_MEMORIA)
            enviar_ip_memoria_a_kernel(*(params->socket), params->path_config);
        else
        {
            obtener_procesoCPU_del_stream(buffer_leer_recv(params->socket), params->procesoCPU);

            llega_proceso();
        }

        free(modulo);
        free(op_code);
    }
}

void *manageINTERRUPT(void *ptr)
{

    parametros_hilo_Cpu *params = ((parametros_hilo_Cpu *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);
        t_buffer *buffer = buffer_leer_recv(params->socket);

        *(params->interrupcion) = buffer_read_uint32(buffer); // pasar un 1 como unsigned int para interrumpir, un 2 para indicar fin de quanntum

        buffer_destroy(buffer);

        free(modulo);
        free(op_code);
    }
}

void obtener_procesoCPU_del_stream(t_buffer *buffer, Contexto_proceso *procesoCPU)
{

    procesoCPU->pid = buffer_read_uint32(buffer);
    procesoCPU->pc = buffer_read_uint32(buffer);
    obtener_registros(buffer, procesoCPU);
    procesoCPU->SI = buffer_read_uint32(buffer);
    procesoCPU->DI = buffer_read_uint32(buffer);

    buffer_destroy(buffer);
}

void obtener_registros(t_buffer *buffer, Contexto_proceso *procesoCPU)

{
    procesoCPU->registros.ax.u8 = buffer_read_uint8(buffer);
    procesoCPU->registros.eax.i32 = buffer_read_uint32(buffer);
    procesoCPU->registros.bx.u8 = buffer_read_uint8(buffer);
    procesoCPU->registros.ebx.i32 = buffer_read_uint32(buffer);
    procesoCPU->registros.cx.u8 = buffer_read_uint8(buffer);
    procesoCPU->registros.ecx.i32 = buffer_read_uint32(buffer);
    procesoCPU->registros.dx.u8 = buffer_read_uint8(buffer);
    procesoCPU->registros.edx.i32 = buffer_read_uint32(buffer);
}

void enviar_ip_memoria_a_kernel(int socket, char *path_config)
{
    t_config *config = config_create(path_config);

    char *ip = config_get_string_value(config, "IP_MEMORIA");
    int size = strlen(ip) + 1;
    t_buffer *buffer = buffer_create(size + 4);
    buffer_add_string(buffer, size, ip);

    enviarMensaje(&socket, buffer, CPU, MENSAJE);

    config_destroy(config);
}