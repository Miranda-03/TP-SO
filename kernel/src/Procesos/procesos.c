#include <Procesos/procesos.h>


t_dictionary  *interfactes_conectadas = dictionary_create();

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;



void planificaciónDeProcesos(){
    t_queue *colaNew = queue_create();
    t_queue *colaReady = queue_create();
    t_queue *colaBlock = queue_create();
    t_queue *colaExec = queue_create();
    t_queue *colaExit = queue_create();

    t_log* logger = iniciar_logger();
    
    do{
        log_info(logger,"Seleccione una operacion: ");
        char *operacion = readline(">");
        
        switch (operacion)
        {
        case "1":
            break;
        case "2":
            iniciar_proceso(colaNew, colaReady);
            enviarproceso();
            break;
        case "3":
            break;
        case "4":
            break;
        case "5":
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

Pcb *crearPcb(int quantum, int duracion, t_queue *colaNew){
    Pcb *pcb = malloc(sizeof(Pcb));
    pcb->PID = 0;
    pcb->program_counter = 0;
    pcb->quantum = quantum;
    pcb->duracion = duracion;
    

    queue_push(colaNew, pcb);

    return pcb;
}

void iniciar_proceso(t_queue* cola_new, t_queue* cola_ready) {
    Pcb* pcb = crearPcb(2,1,cola_new); // el pid, el pc y el quantum vienen de memoria
    queue_push(cola_ready, pcb);
    planificarFIFO(cola_ready);
}

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
    buffer_add(buffer, pcb, 4);
    enviarMensaje(socket, buffer, CPU, PROCESO);
}

