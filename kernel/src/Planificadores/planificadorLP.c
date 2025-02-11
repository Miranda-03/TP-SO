#include "planificadorLP.h"

int pid_global = 0;

// int grado_multiprogramacion;

int *socketBidireccionalMemoria;

pthread_mutex_t mutexPID;
pthread_mutex_t mutexColaNEW;
pthread_mutex_t mutexMSGMemoria;

sem_t hay_procesos_en_new;

t_queue *cola_de_new;
t_queue *cola_de_exit;

t_log *kernel_loger_lp;

sem_t grado_multiprogramacion;

StructMultiprogramacion *control_multiprogramacion;

void inicarPlanificadorLargoPLazo(int socketMemoria, char *path_config)
{
    // sem_init(&grado_multiprogramacion, 0, atoi(obtenerValorConfig(path_config, "GRADO_MULTIPROGRAMACION")));
    control_multiprogramacion = malloc(sizeof(StructMultiprogramacion));

    init_sem_multiprogramacion(control_multiprogramacion, atoi(obtenerValorConfig(path_config, "GRADO_MULTIPROGRAMACION")));

    sem_init(&hay_procesos_en_new, 0, 0);
    cola_de_exit = queue_create();
    cola_de_new = queue_create();
    socketBidireccionalMemoria = malloc(4);
    *socketBidireccionalMemoria = socketMemoria;
    kernel_loger_lp = log_create("logs/kernel_info.log", "plani_lp", 1, LOG_LEVEL_INFO);
    iniciarMutex();
    pthread_t hilo_enviar_procesos_a_ready;
    pthread_create(&hilo_enviar_procesos_a_ready, NULL, agregarNuevoProcesoReady, NULL);
    pthread_detach(hilo_enviar_procesos_a_ready);
}

void init_sem_multiprogramacion(StructMultiprogramacion *msem, int value)
{
    msem->valor_inicial = value;
    msem->procesos_ejecutando = 0;
    sem_init(&(msem->sem_multiprogramacion), 0, value);
    pthread_mutex_init(&(msem->mutex), NULL);
}

void nuevoProceso(char *path_instrucciones)
{
    PcbGuardarEnNEW *nuevo_proceso = (PcbGuardarEnNEW *)malloc(sizeof(PcbGuardarEnNEW));
    nuevo_proceso->proceso = crearProcesoEstadoNEW();
    nuevo_proceso->path_instrucciones = string_new();
    string_append(&(nuevo_proceso->path_instrucciones), path_instrucciones);

    mensaje_nuevo_proceso(nuevo_proceso->proceso->pid);
    agregarProcesoColaNew(nuevo_proceso);
}

void mensaje_nuevo_proceso(int pid)
{
    char *mensaje = string_new();

    string_append(&mensaje, "Se crea el proceso ");
    string_append(&mensaje, string_itoa(pid));
    string_append(&mensaje, " en NEW");

    log_info(kernel_loger_lp, mensaje);

    free(mensaje);
}

void agregarProcesoColaNew(PcbGuardarEnNEW *proceso)
{
    pthread_mutex_lock(&mutexColaNEW);
    queue_push(cola_de_new, proceso);
    pthread_mutex_unlock(&mutexColaNEW);
}

void PLPNuevoProceso(char *path_instrucciones)
{
    nuevoProceso(path_instrucciones);
    sem_post(&hay_procesos_en_new);
}

void *agregarNuevoProcesoReady(void *ptr)
{
    while (1)
    {
        sem_wait(&hay_procesos_en_new);

        if (!queue_is_empty(cola_de_new))
        {
            wait_multiprogramacion();

            wait_flujo();

            PcbGuardarEnNEW *nuevo_proceso = sacarProcesoDeNew();

            int resultadoMemoria = guardarInstruccionesMemoria(nuevo_proceso);

            if (resultadoMemoria > 0)
            {
                agregarProcesoNEWaREADYEnPLANI_CP(nuevo_proceso->proceso);
            }
            else
            {
                post_flujo();
                post_multiprogramacion();
                log_error(kernel_loger_lp, "Error en la memoria");
                nuevo_proceso->proceso->estado = ESTADO_EXIT;
                queue_push(cola_de_exit, nuevo_proceso->proceso);
                log_info(kernel_loger_lp, "Finaliza el proceso %d - Motivo: MEMORY_ERROR (path not found)", nuevo_proceso->proceso->pid);
                free(nuevo_proceso);
            }
        }
    }
}

int guardarInstruccionesMemoria(PcbGuardarEnNEW *proceso)
{
    t_buffer *buffer = buffer_create(12 + strlen(proceso->path_instrucciones) + 1);
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
    free(modulo);
    free(codigo);

    return resultado;
}

PcbGuardarEnNEW *sacarProcesoDeNew()
{
    // sem_wait(&grado_multiprogramacion);
    pthread_mutex_lock(&mutexColaNEW);
    PcbGuardarEnNEW *nuevo_proceso = queue_pop(cola_de_new);
    pthread_mutex_unlock(&mutexColaNEW);

    return nuevo_proceso;
}

Pcb *crearProcesoEstadoNEW()
{
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

void terminarProceso(Pcb *proceso, char *motivo_exit)
{
    proceso->estado = ESTADO_EXIT;
    quitarMemoria(proceso);
    liberar_recursos(proceso);
    queue_push(cola_de_exit, proceso);
    mensaje_exit(proceso->pid, motivo_exit);
    // sem_post(&grado_multiprogramacion);
    post_multiprogramacion();
}

void mensaje_exit(int pid, char *motivo_exit)
{
    char *mensaje = string_new();

    string_append(&mensaje, "Finaliza el proceso ");
    string_append(&mensaje, string_itoa(pid));
    string_append(&mensaje, " - Motivo: ");
    string_append(&mensaje, motivo_exit);

    log_info(kernel_loger_lp, mensaje);

    free(mensaje);
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

    pthread_mutex_unlock(&mutexMSGMemoria);
}

int buscarProcesoEnREADYyEXITporIDyFinalizarlo(char *PID) // HEADER
{
}

void ajustar_grado_multiprogramacion(int new_value)
{
    pthread_mutex_lock(&(control_multiprogramacion->mutex));

    int current_value;
    sem_getvalue(&(control_multiprogramacion->sem_multiprogramacion), &current_value);

    int delta = new_value - (control_multiprogramacion->valor_inicial);

    if (delta > 0)
    {
        for (int i = 0; i < delta; i++)
        {
            sem_post(&(control_multiprogramacion->sem_multiprogramacion));
        }
    }
    /*else
    {
        for (int i = 0; i < -delta; i++)
        {
            int value;
            sem_getvalue(&(control_multiprogramacion->sem_multiprogramacion), &value);
            if (value > 0)
                sem_wait(&(control_multiprogramacion->sem_multiprogramacion));
        }
    }*/

    control_multiprogramacion->valor_inicial = new_value;
    pthread_mutex_unlock(&(control_multiprogramacion->mutex));
}

void wait_multiprogramacion()
{

    if (control_multiprogramacion->procesos_ejecutando > control_multiprogramacion->valor_inicial)
    {
        int esperar = control_multiprogramacion->procesos_ejecutando - control_multiprogramacion->valor_inicial;

        for (int i = 0; i < esperar; i++)
        {
            sem_wait(&(control_multiprogramacion->sem_multiprogramacion));
        }
    }
    else if (control_multiprogramacion->procesos_ejecutando == control_multiprogramacion->valor_inicial)
    {
        int valor;
        sem_getvalue(&(control_multiprogramacion->sem_multiprogramacion), &valor);

        if (valor > 0)
        {
            for (int i = 0; i < valor; i++)
            {
                sem_wait(&(control_multiprogramacion->sem_multiprogramacion));
            }
        }
    }

    sem_wait(&(control_multiprogramacion->sem_multiprogramacion));

    control_multiprogramacion->procesos_ejecutando += 1;
}

void post_multiprogramacion()
{
    int value;
    sem_getvalue(&(control_multiprogramacion->sem_multiprogramacion), &value);
    if (value < control_multiprogramacion->valor_inicial)
        sem_post(&(control_multiprogramacion->sem_multiprogramacion));

    control_multiprogramacion->procesos_ejecutando -= 1;
}

int encontrar_en_new_y_terminar(int pid)
{
    PcbGuardarEnNEW *proceso = NULL;

    bool encontrar_proceso(void *value)
    {
        PcbGuardarEnNEW *proceso = (PcbGuardarEnNEW *)value;
        if (proceso->proceso->pid == pid)
            return 1;
        return 0;
    }

    proceso = list_remove_by_condition(cola_de_new->elements, encontrar_proceso);
    if (proceso != NULL)
    {
        proceso->proceso->estado = ESTADO_EXIT;
        queue_push(cola_de_exit, proceso->proceso);
        log_info(kernel_loger_lp, "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", proceso->proceso->pid);
        free(proceso);
        return 1;
    }
    return -1;
}

void listar_estados_lp()
{
    t_log *loger_estados_lp = log_create("logs/kernel_info.log", "proceso_estado", 1, LOG_LEVEL_INFO);

    char *mensaje_lp_new = string_new();
    string_append(&mensaje_lp_new, "NEW [ ");
    void recorrerNew(void *value)
    {
        PcbGuardarEnNEW *proceso = (PcbGuardarEnNEW *)value;
        string_append(&mensaje_lp_new, string_itoa(proceso->proceso->pid));
        string_append(&mensaje_lp_new, ", ");
    }

    list_iterate(cola_de_new->elements, recorrerNew);
    string_trim_right(&mensaje_lp_new);
    char ultimo = mensaje_lp_new[strlen(mensaje_lp_new) - 1];
    if (mensaje_lp_new[strlen(mensaje_lp_new) - 1] == ',')
        mensaje_lp_new[strlen(mensaje_lp_new) - 1] = '\0';
    string_append(&mensaje_lp_new, " ]");
    log_info(loger_estados_lp, mensaje_lp_new);
    free(mensaje_lp_new);

    char *mensaje_lp_exit = string_new();
    string_append(&mensaje_lp_exit, "EXIT [ ");
    void recorrerExit(void *value)
    {
        Pcb *proceso = (Pcb *)value;
        string_append(&mensaje_lp_exit, string_itoa(proceso->pid));
        string_append(&mensaje_lp_exit, ", ");
    }

    list_iterate(cola_de_exit->elements, recorrerExit);
    string_trim_right(&mensaje_lp_exit);
    if (mensaje_lp_exit[strlen(mensaje_lp_exit) - 1] == ',')
        mensaje_lp_exit[strlen(mensaje_lp_exit) - 1] = '\0';
    string_append(&mensaje_lp_exit, " ]");
    log_info(loger_estados_lp, mensaje_lp_exit);
    free(mensaje_lp_exit);

    log_destroy(loger_estados_lp);
}
