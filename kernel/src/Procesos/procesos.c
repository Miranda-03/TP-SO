#include <Procesos/procesos.h>


t_dictionary  *interfactes_conectadas = dictionary_create();

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;



void planificacionDeProcesos(int16_t operacion){
    t_queue *colaNew = queue_create();
    t_queue *colaReady = queue_create();
    t_queue *colaBlock = queue_create();
    t_queue *colaExec = queue_create();
    t_queue *colaExit = queue_create();

    t_log* logger = iniciar_logger();
    
    do{
        
        switch (operacion)
        {
        case "1":
            break;
        case "2":
            iniciar_proceso(colaNew);
            break;
        case "3":
            proceso_ready(colaNew,colaReady);
            break;
        case "4":
            ejecutar_proceso(colaReady,colaExec);
            break;
        case "5":
            finalizar_proceso(colaExit,colaExec,colaBlock,logger);
            proceso_ready(colaNew,colaReady);
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

Pcb *crearPcb(){
    Pcb *pcb = malloc(sizeof(Pcb));
    pcb->pid = asignar_pid();
    pcb->quantum = obtenerValorConfig("./kernel.config");

    return pcb;
}

void iniciar_proceso(t_queue* cola_new) {
    Pcb* pcb = crearPcb(); // el pid, el pc y el quantum vienen de memoria
    Proceso* proceso = crear_proceso(pcb);
    queue_push(cola_new, proceso);
}

void crear_proceso(Pcb* pcb)
{
    Proceso* proceso = malloc(sizeof(Proceso));
    proceso->pcb=pcb;
    proceso->contexto=NEW;
}

proceso_ready(t_queue* colaNew,t_queue* colaReady)
{
    Proceso* proceso=queue_peek(colaNew);
    enviarproceso(,proceso);//Envia el proceso a memoria y lo recibe
    queue_push(colaReady,proceso);
    queue_pop(colaNew);
}
ejecutar_proceso(t_queue* colaready,t_queue* colaexec)
{
 Proceso* proceso=queue_peek(colaready);
 enviarproceso(,proceso);
 queue_push(colaexec,proceso);
 queue_pop(colaready);
}
finalizar_proceso(t_queue*colaexit,t_queue*colaexec,t_queue* colablock,t_log* log)
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

void enviarproceso(int *KernelSocketCPUDispatch, Proceso *proceso, pthread_mutex_t mutex;){
    /*t_buffer *buffer = buffer_create(sizeof(int64_t)); //Tal vez hay que crear un struct que tenga el pcb
    buffer_add(buffer, proceso, 4);
    enviarMensaje(socket, buffer, CPU, PROCESO); */
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