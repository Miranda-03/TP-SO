#include "CPUConexion.h"
#include "Globales/globales.h"


void *manageDISPATCH(void *ptr)
{

    parametros_hilo_Kernel *params = ((parametros_hilo_Kernel *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);

        obtener_procesoCPU_del_stream(buffer_leer_recv(params->socket),params->motivo,params->pid,params->registros,params->instruccion);
    }
}

void *manageINTERRUPT(void *ptr)
{

    parametros_hilo_Cpu *params = ((parametros_hilo_Cpu *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);

        obtener_procesoCPUint_del_stream(buffer_leer_recv(params->socket),params->motivo,params->pid,params->registros,params->instruccion);
        
    }
}

void obtener_procesoCPU_del_stream(t_buffer *buffer, MotivoDesalojo* motivo, int* pid, Registros* registros, char* instruccion)
{
    motivo = buffer_read_uint32(buffer);
    pid = buffer_read_uint32(buffer);
    obtener_registros(buffer,registros);
    buffer_destroy(buffer);
    manejar_proceso(motivo,pid,registros,instruccion);
}
void obtener_procesoCPUint_del_stream(t_buffer *buffer, MotivoDesalojo* motivo, int* pid, Registros* registros,char* instruccion)
{
    motivo = buffer_read_uint32(buffer);
    pid = buffer_read_uint32(buffer);
    obtener_registros(buffer,registros);
    instruccion=buffer_read_string(buffer,strlen(instruccion));
    buffer_destroy(buffer);
    manejar_proceso(motivo,pid,registros,instruccion);
}

void obtener_registros(t_buffer *buffer, Registros* registros)
{
    registros->registros.pc = buffer_read_uint32(buffer);
    registros->registros.ax = buffer_read_uint8(buffer);
    registros->registros.eax = buffer_read_uint32(buffer);
    registros->registros.bx = buffer_read_uint8(buffer);
    registros->registros.ebx = buffer_read_uint32(buffer);
    registros->registros.cx = buffer_read_uint8(buffer);
    registros->registros.ecx = buffer_read_uint32(buffer);
    registros->registros.dx = buffer_read_uint8(buffer);
    registros->registros.edx = buffer_read_uint32(buffer);
}

