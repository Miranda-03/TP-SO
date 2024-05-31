#include <Procesos/procesos.h>


t_dictionary  *interfactes_conectadas = dictionary_create();

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;



void planificacionDeProcesos(int case,char* path)
{
    t_list *listaNew = list_create();
    t_list *listaReady = list_create();
    t_list *listaBlock = list_create();
    t_list *listaExec = list_create();
    t_list *listaExit = list_create();

    t_log* logger = iniciar_logger();
    
    do{
        log_info(logger,"Seleccione una operacion: ");
        char *operacion = readline(">");
        
        switch (operacion)
        {
        case "1":
            break;
        case "2":
            iniciar_proceso(listaNew);
            break;
        case "3":
            proceso_ready(listaNew,listaReady);
            break;
        case "4":
            ejecutar_proceso(listaReady,listaExec);
            break;
        case "5":
            finalizar_proceso(listaNew,listaReady,listaExec,listaBlock,listaExit,logger);
            proceso_ready(listaNew,listaReady);
            break;
        case "6":
            break;
        case "exit":
            exit(1);
            break;
        default:
            log_info(logger,"Operación Inválida ");
            break;
        }
    }while (true);
    log_destroy(logger);

}

Pcb *crearPcb(int quantum){
    Pcb *pcb = malloc(sizeof(Pcb));
    pcb->pid = asignar_pid();
    pcb->quantum = obtenerValorConfig("./kernel.config","QUANTUM");
    pcb->registros=NULL;

    return pcb;
}

void iniciar_proceso(t_list* list_new) {
    Pcb* pcb = crearPcb(2); 
   list_add(list_new,pcb);
}
proceso_ready(t_list* listNew,t_list* listReady,char* path)
{
   int grado_multiprogamacion = config_get_string_value("./kernel.config","GRADO_MULTIPROGRAMACION");
   if(list_size(listReady)< grado_multiprogamacion)
   {
   Pcb* pcb=list_remove(listNew,0);
   t_buffer* buffer= buffer_create(sizeof(unsigned int)+strlen(path));
   buffer_add(buffer,proceso->pcb->pid,sizeof(unsigned int));
   buffer_add(buffer,path,sizeof(char*));
   enviarMensaje(socket,buffer,KERNEL,PROCESO);
   list_add(listReady,pcb);
   }
}

ejecutar_proceso(t_list* listready,t_list* listexec)
{
  if(list_size(listexec)==0)
   {
    Pcb* pcb=list_remove(listready,0);
    t_buffer* buffer = buffer_create(sizeof(unsigned int)+sizeof(Registros));
    buffer_add(buffer,proceso->pcb->pid,sizeof(unsigned int));
    buffer_add(buffer,proceso->pcb->registros.pc,sizeof(uint32_t));
    buffer_add(buffer,proceso->pcb->registros.ax,sizeof(uint8_t));
    buffer_add(buffer,proceso->pcb->registros.eax,sizeof(uint32_t));
    buffer_add(buffer,proceso->pcb->registros.bx,sizeof(uint8_t));
    buffer_add(buffer,proceso->pcb->registros.ebx,sizeof(uint32_t));
    buffer_add(buffer,proceso->pcb->registros.cx,sizeof(uint8_t));
    buffer_add(buffer,proceso->pcb->registros.ecx,sizeof(uint32_t));
    buffer_add(buffer,proceso->pcb->registros.dx,sizeof(uint8_t));
    buffer_add(buffer,proceso->pcb->registros.edx,sizeof(uint32_t));
    enviarMensaje(socket,buffer,KERNEL,PROCESO);
    list_add(listexec,pcb);
   }
}

/*finalizar_proceso(t_list* listNew,t_list* listReady,t_list* listExec, t_list* listBlock,t_list* listExit,t_log* log)
{
 do{
    log_info(log,"Seleccione la cola");
     char *operacion = readline(">");
     switch (operacion)
        {
        case "1":
        Proceso* proceso=queue_peek(colaexec);
        queue_push(colaexit,proceso);
        queue_pop(colaexec);
        exit(1);
            break;
        case"2":
        Proceso* proceso=queue_peek(colablock);
        queue_push(colaexit,proceso);
        queue_pop(colablock);
        exit(1);
        break;
         default:
            log_info(log,"Operación Inválida ");
            break;
        }
    }while (true);
    log_destroy(log);
 }*/

void planificarFIFO(t_queue *cola){
    while (!queue_is_empty(cola)) {
        Pcb* pcb = queue_peek(cola);
        printf("Proceso de pid %d y quantum %d\n", pcb->PID, pcb->quantum);
        queue_pop(cola);
    }
}

void planificarRR(t_queue *cola){

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
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("cliente.log","CL_LOG",1,LOG_LEVEL_INFO);
	if(nuevo_logger == NULL){
		printf("Error al crear el log");
		exit (1);
	}

	return nuevo_logger;
}

void enviarproceso(int *KernelSocketCPUDispatch, Pcb *pcb){
    t_buffer *buffer = buffer_create(sizeof(int64_t)); //Tal vez hay que crear un struct que tenga el pcb
    buffer_add(buffer, pcb, sizeof(Pcb));
    enviarMensaje(socket, buffer, CPU, PROCESO);
}

int asignar_pid(int pid)
{
int valor;
pthread_mutex_lock(&mutex);
valor = pid;
pid++;
pthread_mutex_unlock(&mutex);
return pid;
}
