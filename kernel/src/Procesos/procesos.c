#include "procesos.h"

void iniciar_proceso(char* path){
   if(list_size(listaReady)< grado_multiprogamacion)
   {  
       t_buffer* buffer = buffer_create(sizeof(uint32_t)*2+strlen(path));
       Pcb* pcb = crearPcb();
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
   else{
    return -1;
   }

}

Pcb *crearPcb(){
   Pcb *pcb = malloc(sizeof(Pcb));
   pcb->pid = asignar_pid();
   pcb->quantum = quantum_global;
   pcb->registros.ax = 0;
   pcb->registros.bx = 0;
   pcb->registros.cx = 0;
   pcb->registros.dx = 0;
   pcb->registros.eax = 0;
   pcb->registros.ebx = 0;
   pcb->registros.ecx = 0;
   pcb->registros.edx = 0;
   pcb->registros.pc = 0;
   return pcb;
}

void llenarbuffer(t_buffer* buffer,Pcb* pcb)
{
   buffer_add(buffer,pcb->pid,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.pc,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.ax,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.eax,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.bx,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.ebx,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.cx,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.ecx,sizeof(uint32_t));
   buffer_add(buffer,pcb->registros.dx,sizeof(uint8_t));
   buffer_add(buffer,pcb->registros.edx,sizeof(uint32_t));
}

void finalizar_proceso(int pid,Registros* registros) //no esta terminada
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
void manejar_proceso(MotivoDesalojo *motivo, int pid, Registros* registros,char* instruccion)
{
    if(motivo==FIN_DE_QUANNTUM)
    {
        Pcb* pcb=list_get(listaExec,0);
        list_remove(listaExec,0);
        //Si hay VRR resta el quantum del pcb por el asignado por el sistema y envia el proceso a la lista de mayor prioridad
        pcb->registros=registros;
        list_add(listaReady,pcb);
        enviarcpu();
    }
    else if(motivo==INTERRUPCION_IO)
    {
        Pcb* pcb=list_get(listaExec,0);
        list_remove(listaExec,0);
        pcb->registros=registros;
        //Manda a hacer la IO
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
    Pcb* pcb=list_get(listaReady,0);
    list_remove(listaReady,0);
    llenarbuffer(buffer,pcb);
    enviarMensaje(KernelSocketCPUDispatch,buffer,KERNEL,PROCESO);
    //Si hay RR empieza a contar el quantum que tiene el pcb
    list_add(listaExec,pcb);
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




/* proceso_ready(t_list* listNew,t_list* listReady,char* path) // No hace falta
{
  int grado_multiprogamacion = config_get_string_value("./kernel.config","GRADO_MULTIPROGRAMACION");
  if(list_size(listReady)< grado_multiprogamacion)
  {
  Pcb* pcb=list_remove(listNew,0);
  t_buffer* buffer= buffer_create(sizeof(unsigned int)+strlen(path));
  buffer_add(buffer,pcb->pid,sizeof(unsigned int));
  buffer_add(buffer,path,sizeof(char*));
  enviarMensaje(socket,buffer,KERNEL,PROCESO);
  list_add(listReady,pcb);
  }
} */
/*void planificarFIFO(t_queue *cola){
   while (!queue_is_empty(cola)) {
       Pcb* pcb = queue_peek(cola);
       printf("Proceso de pid %d y quantum %d\n", pcb->PID, pcb->quantum);
       queue_pop(cola);
   }
} */


/* void planificarRR(t_queue *cola){


   while (!queue_is_empty(cola)) {
       Pcb* pcb = queue_peek(cola);
       printf("Proceso de pid %d y quantum %d\n", pcb->PID, pcb->quantum);
       pcb->duracion -= pcb->quantum;
       if (pcb->duracion > 0) {
           queue_push(cola, pcb);
       } else {
           queue_pop(cola);
       }
   }
} */

