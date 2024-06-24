#include "planificadorLP.h"

int pid_global = -1;

// int grado_multiprogramacion;

int *socketBidireccionalMemoria;

pthread_mutex_t mutexPID;
pthread_mutex_t mutexColaNEW;
pthread_mutex_t mutexMSGMemoria;
sem_t grado_multiprogramacion;

t_queue *cola_de_new;
t_queue *cola_de_exit;

void inicarPlanificadorLargoPLazo(int socketMemoria)
{
    sem_init(&grado_multiprogramacion, 0, atoi(obtenerValorConfig(PATH_CONFIG, "GRADO_MULTIPROGRAMACION")));
    cola_de_exit = queue_create();
    cola_de_new = queue_create();
    socketBidireccionalMemoria = malloc(4);
    *socketBidireccionalMemoria = socketMemoria;
    iniciarMutex();
}

void nuevoProceso(char *path_instrucciones)
{
    PcbGuardarEnNEW *nuevo_proceso = (PcbGuardarEnNEW *)malloc(sizeof(PcbGuardarEnNEW));
    nuevo_proceso->proceso = crearProcesoEstadoNEW();
    nuevo_proceso->path_instrucciones = path_instrucciones;
    agregarProcesoColaNew(nuevo_proceso);
}

void agregarProcesoColaNew(PcbGuardarEnNEW *proceso)
{
    pthread_mutex_lock(&mutexColaNEW);
    queue_push(cola_de_new, proceso);
    pthread_mutex_unlock(&mutexColaNEW);
}

void *PLPNuevoProceso(void *ptr) // es una funcion de un hilo
{
    char *path_instrucciones = (char *)ptr;
    nuevoProceso(path_instrucciones);
    agregarNuevoProcesoReady();
    pthread_exit(NULL);
}

void agregarNuevoProcesoReady()
{
    if (!queue_is_empty(cola_de_new))
    {
        sem_wait(&grado_multiprogramacion);
        PcbGuardarEnNEW *nuevo_proceso = sacarProcesoDeNew();

        int resultadoMemoria = guardarInstruccionesMemoria(nuevo_proceso);
        printf("El resultado de la memoria despues de guardar la instruccion es: %d\n", resultadoMemoria);
        if (resultadoMemoria > 0)
            agregarProcesoColaReady(nuevo_proceso->proceso);
    }
}

int guardarInstruccionesMemoria(PcbGuardarEnNEW *proceso)
{
    t_buffer *buffer = buffer_create(8 + strlen(proceso->path_instrucciones) + 1);
    buffer_add_uint32(buffer, 1);
    buffer_add_uint32(buffer, proceso->proceso->pid);
    buffer_add_string(buffer, strlen(proceso->path_instrucciones) + 1, proceso->path_instrucciones);
    int resultado;
    enviarMensajeMemoria(buffer, &resultado);
    return resultado;
}

int esperarRespuesteDeMemoria()
{
    TipoModulo *modulo = get_modulo_msg_recv(socketBidireccionalMemoria);
    op_code *codigo = get_opcode_msg_recv(socketBidireccionalMemoria);

    t_buffer *buffer = buffer_leer_recv(socketBidireccionalMemoria);

    int resultado = buffer_read_uint32(buffer);

    buffer_destroy(buffer);

    return resultado;
}

PcbGuardarEnNEW *sacarProcesoDeNew()
{
    pthread_mutex_lock(&mutexColaNEW);
    PcbGuardarEnNEW *nuevo_proceso = queue_pop(cola_de_new);
    pthread_mutex_unlock(&mutexColaNEW);

    return nuevo_proceso;
}

Pcb *crearProcesoEstadoNEW()
{
    /*
    uint32_t pid;
    uint32_t pc;
    uint32_t quantumRestante;
    Registros registros;
    EstadoProceso estado;
    */
    Pcb *pcb = malloc(sizeof(Pcb));
    pcb->pid = asignar_pid();
    pcb->pc = 0;
    pcb->quantumRestante = 0;
    pcb->estado = NEW;
    pcb->registros.ax.u8 = 0;
    pcb->registros.bx.u8 = 0;
    pcb->registros.cx.u8 = 0;
    pcb->registros.dx.u8 = 0;
    pcb->registros.eax.i32 = 0;
    pcb->registros.ebx.i32 = 0;
    pcb->registros.ecx.i32 = 0;
    pcb->registros.edx.i32 = 0;

    return pcb;
}

int asignar_pid()
{
    pthread_mutex_lock(&mutexPID);
    pid_global++;
    pthread_mutex_unlock(&mutexPID);
    return pid_global;
}

void iniciarMutex()
{
    pthread_mutex_init(&mutexPID, NULL);
    pthread_mutex_init(&mutexColaNEW, NULL);
    pthread_mutex_init(&mutexMSGMemoria, NULL);
}

void terminarProceso(Pcb *proceso) // FALTA LIBERAR LOS RECURSOS
{
    quitarMemoria(proceso);
    free(proceso);
    sem_post(&grado_multiprogramacion);
}

void quitarMemoria(Pcb *proceso)
{
    t_buffer *buffer = buffer_create(8);
    buffer_add_uint32(buffer, 0);
    buffer_add_uint32(buffer, proceso->pid);
    int resultado;
    enviarMensajeMemoria(buffer, &resultado);
}

void enviarMensajeMemoria(t_buffer *buffer, int *resultado)
{
    pthread_mutex_lock(&mutexMSGMemoria);

    enviarMensaje(socketBidireccionalMemoria, buffer, KERNEL, MENSAJE);

    *resultado = esperarRespuesteDeMemoria();

    printf("La respuesta es: %u\n", *resultado);

    pthread_mutex_unlock(&mutexMSGMemoria);
}