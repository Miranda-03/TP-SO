#include "procesos.h"

void iniciar_proceso(char* path){ 
    Pcb* pcb = crearPcb();
    queue_push(cola_new, pcb);  
    pcb = (Pcb*) queue_peek(cola_new);
    t_buffer* buffer = buffer_create(sizeof(uint32_t)*2+strlen(path));
    buffer_add(buffer,1,sizeof(uint32_t));
    buffer_add(buffer,pcb->pid,sizeof(uint32_t));
    buffer_add(buffer,path,sizeof(path));
    enviarMensaje(KernelSocketMemoria,buffer,KERNEL,PROCESO);   
   
}

void procesos_a_ready(){
    Pcb* pcb = (Pcb*) queue_pop(cola_new);
    if (queue_size<grado_multiprogamacion){
        pcb->contexto = READY;
        queue_push(cola_ready);
        /*log_info(logger_kernel,"Cola Ready / Ready Prioridad: \n");
        list_iterate(listaReady, loggearLista); ver si se puede hacer de una forma facil*/ 
    }
    else{
        //log_info(logger_kernel,"Cola Ready llena");
    }
}

/*void iniciar_proceso(char* path){
   
   Pcb* pcb = crearPcb();
   queue_push(cola_new, pcb);
   if(list_size(listaReady)< grado_multiprogamacion)
   {  
        pcb = (Pcb*) queue_peek(cola_new);
        queue_pop(cola_new);
        t_buffer* buffer = buffer_create(sizeof(uint32_t)*2+strlen(path));
        buffer_add(buffer,1,sizeof(uint32_t));
        buffer_add(buffer,pcb->pid,sizeof(uint32_t));
        buffer_add(buffer,path,sizeof(path));
        enviarMensaje(KernelSocketMemoria,buffer,KERNEL,PROCESO);
        list_add(listaReady,pcb);
        log_info(logger_kernel,"Cola Ready / Ready Prioridad: \n");
        list_iterate(listaReady, loggearLista);
        if(list_size(listaExec)==0)
        {
        enviarcpu();
        }
   }
   
}*/

//void

Pcb *crearPcb(){
   Pcb *pcb = malloc(sizeof(Pcb));
   pcb->pid = asignar_pid();
   pcb->quantum = quantum_global;
   pcb->registros.ax.u8 = 0;
   pcb->registros.bx.u8 = 0;
   pcb->registros.cx.u8 = 0;
   pcb->registros.dx.u8 = 0;
   pcb->registros.eax.i32 = 0;
   pcb->registros.ebx.i32 = 0;
   pcb->registros.ecx.i32= 0;
   pcb->registros.edx.i32 = 0;
   pcb->registros.pc = 0;
   pcb->contexto = ESTADO_EXIT;
   return pcb;
}

void llenarbuffer(t_buffer* buffer,Pcb* pcb){
   buffer_add(buffer,pcb->pid,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.pc,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.ax.u8,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.eax.i32,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.bx.u8,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.ebx.i32,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.cx.u8,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.ecx.i32,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.dx.u8,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.edx.i32,sizeof(uint32_t));
}

void finalizar_proceso(int pid,Registros registros) //no esta terminada
{
    Pcb* pcb=NULL;
    pcb=list_find(listaExec,pcb->pid==pid);
    if(pcb!=NULL)
    {
        t_buffer* buffer=buffer_create(sizeof(uint32_t)*2);
        buffer_add(buffer,-1,sizeof(uint32_t));
        buffer_add(buffer,pid,sizeof(uint32_t));
        list_remove_by_condition(listaExec,pcb->pid==pid);
        enviarMensaje(KernelSocketMemoria,buffer,KERNEL,PROCESO);
        enviarcpu(); 
        // Aca si sacas el pcb de la listaExec podes ver los registros que tenia antes y despues de la ejecucion 
        //log_info(logger_kernel,"PID: <PID> - Estado Anterior: <ESTADO_ANTERIOR> - Estado Actual: <ESTADO_ACTUAL>\n");
        return;
    }
    pcb=list_find(listaReady,pcb->pid==pid);
    if(pcb!=NULL)
    {
     t_buffer* buffer=buffer_create(sizeof(uint32_t)*2);
        buffer_add(buffer,-1,sizeof(uint32_t));
        buffer_add(buffer,pid,sizeof(uint32_t));    
    list_remove_by_condition(listaReady,pcb->pid==pid);
    enviarMensaje(KernelSocketMemoria,buffer,KERNEL,PROCESO);
    return;
    }
    pcb=list_find(listaBlock,pcb->pid==pid);
    if(pcb!=NULL)
    {
        t_buffer* buffer=buffer_create(sizeof(uint32_t)*2);
        buffer_add(buffer,-1,sizeof(uint32_t));
        buffer_add(buffer,pid,sizeof(uint32_t));    
    list_remove_by_condition(listaBlock,pcb->pid==pid);  
    enviarMensaje(KernelSocketMemoria,buffer,KERNEL,PROCESO);
    return;
    }
    //log_info(logger_kernel,"Proceso no encontrado")
}

void manejar_proceso(MotivoDesalojo *motivo, int pid, Registros registros,instruccionIO* instruccion)
{
    if(motivo==FIN_DE_QUANNTUM)
    {
        Pcb* pcb=list_get(listaExec,0);
        list_remove(listaExec,0);
        pcb->registros=registros;
        if(algoritmo_actual==VRR)
        {
            pcb->quantum-=quantum_global;
            list_add(listaQuantum,pcb);
        }
        else{
        list_add(listaReady,pcb);
        }
        enviarcpu();
    }
    else if(motivo==INTERRUPCION_IO)
    {
        Pcb* pcb=list_get(listaExec,0);
        list_remove(listaExec,0);
        pcb->registros=registros;
        t_buffer *buffer = buffer_create(sizeof(uint32_t)*2);
        buffer_add(buffer,pid,sizeof(pid));
        buffer_add(buffer,instruccion,sizeof(instruccion));
        enviarMensaje(KernelsocketIO,buffer,KERNEL,PROCESO);
        list_add(listaBlock,pcb);
        enviarcpu();
    }
    else if(motivo==INTERRUPCION_KERNEL)
    {
        finalizar_proceso(pid,registros);
    }
    else if(motivo==ERROR_DE_PROGRAMA)
    {

    }
    else if(motivo==EXIT_SIGNAL)
    {

    }
}

void enviarcpu()
{
    t_buffer* buffer=buffer_create(sizeof(uint32_t)+sizeof(Registros));
    Pcb* pcb=NULL;
    pcb=list_get(listaQuantum,0);
    if(pcb!=NULL)
    {
        list_remove(listaQuantum,0);
    }
    else
    {
    pcb=list_get(listaReady,0);
    if(pcb!=NULL)
    {
    list_remove(listaReady,0);
    }
    }
    if(pcb!=NULL)
    {
    llenarbuffer(buffer,pcb);
    enviarMensaje(KernelSocketCPUDispatch,buffer,KERNEL,PROCESO);
    //Si hay RR empieza a contar el quantum que tiene el pcb
    list_add(listaExec,pcb);
    }
    else{
        //No hay procesos a ejecutar
    }
}

int asignar_pid(){
    int valor;
    //pthread_mutex_lock(&mutex);
    valor = pid_global;
    pid_global++;
    //pthread_mutex_unlock(&mutex);
    return valor;
}

void loggearLista(void *elemento) {
    Pcb *pcb = (Pcb *)elemento;
    log_info(logger_kernel, "%d\n", pcb->pid);
}

void cambiarGrado(int grado){
    //detern planificacion
    grado_multiprogamacion = grado;
    //reanudar planificacion
}


void eliminar_proceso(){
    Pcb* pcb = (Pcb*) queue_pop(cola_new);
    if(pcb->contexto == ESTADO_EXIT){
        free(pcb);
    }
    else{

    }
    
}