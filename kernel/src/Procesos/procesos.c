#include "procesos.h"

void iniciar_proceso(t_list* list_ready,t_list* list_exec,char* path){
   if(list_size(list_ready)< grado_multiprogamacion)
   {  
       t_buffer* bufferm = buffer_create(sizeof(uint32_t)*2+strlen(path));
       Pcb* pcb = crearPcb();
       buffer_add(bufferm,1,sizeof(uint32_t));
       buffer_add(bufferm,pcb->pid,sizeof(uint32_t));
       buffer_add(bufferm,path,sizeof(path));
       enviarMensaje(KernelSocketMemoria,bufferm,KERNEL,PROCESO);
       if(list_size(list_exec)==0)
       {
       t_buffer* buffercpu = buffer_create(sizeof(uint32_t)+sizeof(Registros)); 
       llenarbuffer(buffercpu,pcb);
       enviarMensaje(KernelSocketCPUDispatch,buffercpu,KERNEL,PROCESO);
       list_add(list_exec,pcb);
       }
       else{
            list_add(list_ready,pcb);
            log_info(logger_kernel,"Cola Ready / Ready Prioridad: \n");
            list_iterate(listaReady, loggearLista);
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

void finalizar_proceso(t_list* listReady,t_list* listExec, t_list* listBlock,int pid) //no esta terminada
{
    Pcb* pcb=NULL;
    pcb=list_find(listExec,pcb->pid==pid);
    if(pcb!=NULL)
    {
        t_buffer* buffercpu = buffer_create(sizeof(uint32_t)+sizeof(Registros));
        t_buffer* buffermem =buffer_create(sizeof(uint32_t)*2);
        buffer_add(buffermem,-1,sizeof(uint32_t));
        buffer_add(buffermem,pid,sizeof(uint32_t));
        list_remove_by_condition(listExec,pcb->pid==pid); 
        //log_info(logger_kernel,"PID: <PID> - Estado Anterior: <ESTADO_ANTERIOR> - Estado Actual: <ESTADO_ACTUAL>\n");
        pcb=list_get(listReady,0);
        list_remove(listReady,0);
        llenarbuffer(buffercpu,pcb);
        enviarMensaje(KernelSocketCPUDispatch,buffercpu,KERNEL,PROCESO);
    }
    pcb=list_find(listReady,pcb->pid==pid);
    if(pcb!=NULL)
    {
    list_remove_by_condition(listReady,pcb->pid==pid);
    }
    pcb=list_find(listaBlock,pcb->pid==pid);
    if(pcb!=NULL)
    {
    list_remove_by_condition(listBlock,pcb->pid==pid);  
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
void reingresar_proceso(MotivoDesalojo* motivo,Pcb* pcb);
{
    if(motivo==FIN_DE_QUANNTUM)
    {
      list_add(listaQuantum,pcb);  
    }
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
