#include "CPUConexion.h"
#include "Globales/globales.h"


void *manageDISPATCH(void *ptr)
{

    parametros_hilo_Kernel *params = ((parametros_hilo_Kernel *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);

        obtener_procesoCPU_del_stream(buffer_leer_recv(params->socket),params->motivo,params->pid,params->registros);
    }
}

void *manageINTERRUPT(void *ptr)
{

    parametros_hilo_Cpu *params = ((parametros_hilo_Cpu *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);
        t_buffer *buffer = buffer_leer_recv(socket);

        params->interrupcion = buffer_read_uint32(buffer);// pasar un 1 como unsigned int para interrumpir, un 2 para indicar fin de quanntum

        buffer_destroy(buffer);
    }
}

void obtener_procesoCPU_del_stream(t_buffer *buffer, MotivoDesalojo* motivo, int* pid, Registros* registros)
{
    Pcb* pcb;
    motivo = buffer_read_uint32(buffer);
    pid = buffer_read_uint32(buffer);
    obtener_registros(buffer,registros);
    buffer_destroy(buffer);

    pcb = actualizarPcb(pcb,Registros);
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

void actualizarPcb(Pcb* pcb, Registros* registros){
    
    pcb->registros.ax = registros.ax;
    pcb->registros.bx = registros.bx ;
    pcb->registros.cx = registros.cx;
    pcb->registros.dx = registros.dx;
    pcb->registros.eax = registros.eax;
    pcb->registros.ebx = registros.ebx;
    pcb->registros.ecx = registros.ecx;
    pcb->registros.edx = registros.ecx;
    pcb->registros.pc = registros.pc;
}