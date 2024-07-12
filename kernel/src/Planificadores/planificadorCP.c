#include "planificadorCP.h"

ParamsPCP_kernel *params;
int PIDprocesoEjecutando;
t_temporal *tiempoEjecutando;
int quantumTotal;
int quantumRestante;
int *PIDbuscadoParaTerminar;

Algoritmo algoritmo;

t_log *kernel_loger_cp;

Pcb *proceosPCB_HILO_recursos;

t_queue *cola_de_ready;
t_queue *cola_de_mayor_prioridad;
t_queue *lista_bloqueados_generico;
sem_t esperando_generico_sem;
t_queue *lista_bloqueados_STDIN;
sem_t esperando_stdin_sem;
t_queue *lista_bloqueados_STDOUT;
sem_t esperando_stdout_sem;
t_queue *lista_bloqueados_DialFS;
sem_t esperando_dialfs_sem;

t_list *lista_de_procesos_con_recursos;
t_list *todas_las_listasBloqueadosPorIDio;

t_dictionary *interfaces_conectadas;
t_dictionary *recursos;

pthread_mutex_t mutexReady;
pthread_mutex_t mutexMayorPriordad;
pthread_mutex_t mutexIOGenerico;
pthread_mutex_t mutexIOSTDIN;
pthread_mutex_t mutexIOSTDOUT;
pthread_mutex_t mutexIODIALFS;
pthread_mutex_t mutexListaTodosPorIDio;

sem_t flujoPlanificador_cp;
sem_t esperar_proceso;
sem_t esperar_guardar_proceso;
sem_t cant_procesos_ready;

MensajeProcesoDelCPU *procesoDelCPU;

void *planificarCortoPlazo(void *ptr)
{
    params = (ParamsPCP_kernel *)ptr; // PARAMS FREE
    interfaces_conectadas = params->interfaces_conectadas;
    recursos = params->recursos;
    algoritmo = params->algoritmo;
    PIDprocesoEjecutando = -1;

    t_config *config = config_create(PATH_CONFIG);
    quantumTotal = atoi(config_get_string_value(config, "QUANTUM"));
    config_destroy(config);

    quantumRestante = quantumTotal;

    kernel_loger_cp = log_create("logs/kernel_info.log", "plani_cp", 1, LOG_LEVEL_INFO); // DESTRUIR

    PIDbuscadoParaTerminar = malloc(4); // FREE
    *PIDbuscadoParaTerminar = -1;

    procesoDelCPU = malloc(sizeof(MensajeProcesoDelCPU)); // FREE
    procesoDelCPU->pcb = malloc(sizeof(Pcb));             // FREE

    cola_de_ready = queue_create();
    cola_de_mayor_prioridad = queue_create();

    lista_bloqueados_generico = queue_create();
    sem_init(&esperando_generico_sem, 0, 0);
    lista_bloqueados_STDIN = queue_create();
    sem_init(&esperando_stdin_sem, 0, 0);
    lista_bloqueados_STDOUT = queue_create();
    sem_init(&esperando_stdout_sem, 0, 0);
    lista_bloqueados_DialFS = queue_create();
    sem_init(&esperando_dialfs_sem, 0, 0);

    lista_de_procesos_con_recursos = list_create();
    todas_las_listasBloqueadosPorIDio = list_create();

    pthread_mutex_init(&mutexReady, NULL);
    pthread_mutex_init(&mutexIOGenerico, NULL);
    pthread_mutex_init(&mutexIOSTDIN, NULL);
    pthread_mutex_init(&mutexIOSTDOUT, NULL);
    pthread_mutex_init(&mutexIODIALFS, NULL);
    pthread_mutex_init(&mutexMayorPriordad, NULL);
    pthread_mutex_init(&mutexListaTodosPorIDio, NULL);

    sem_init(&flujoPlanificador_cp, 0, 1);
    sem_init(&esperar_proceso, 0, 0);
    sem_init(&esperar_guardar_proceso, 0, 0);
    sem_init(&cant_procesos_ready, 0, 0);

    pthread_t hiloParaBloqueados;
    pthread_create(&hiloParaBloqueados, NULL, manageBloqueados, NULL);
    pthread_detach(hiloParaBloqueados);

    crearHilosParaWAIT();

    while (1)
    {
        sem_wait(&cant_procesos_ready);

        if (algoritmo == VRR)
        {
            if (hayProcesosPrioritarios() > 0)
            {
                quantumRestante = enviarProcesoMayorPrioridadCPU();
            }
            else
            {
                quantumRestante = quantumTotal;
                enviarProcesoReadyCPU();
            }
        }
        else
        {
            quantumRestante = quantumTotal;
            enviarProcesoReadyCPU();
        }

        esperarProcesoCPU(quantumRestante);

        // Crear el proceso (PCB) con un malloc
        Pcb *procesoPCB = malloc(sizeof(Pcb));
        procesoPCB->pc = procesoDelCPU->pcb->pc;
        procesoPCB->pid = procesoDelCPU->pcb->pid;
        procesoPCB->quantumRestante = procesoDelCPU->pcb->quantumRestante;
        procesoPCB->estado = EXEC;
        procesoPCB->SI = procesoDelCPU->pcb->SI;
        procesoPCB->DI = procesoDelCPU->pcb->DI;
        guardarLosRegistros(procesoPCB);

        proceosPCB_HILO_recursos = procesoPCB;

        sem_wait(&esperar_guardar_proceso);

        if (chequearMotivoIO(procesoPCB) < 0 && chequearMotivoExit(procesoPCB) < 0 && chequearRecursos(procesoPCB) < 0)
        {
            agregarProcesoAReadyCorrespondiente(procesoPCB);
        }

        mensaje_desalojo();
    }
}

int hayProcesosEnCola()
{
    int respuesta = -1;
    pthread_mutex_lock(&mutexReady);
    if (!queue_is_empty(cola_de_ready))
        respuesta = 1;
    pthread_mutex_unlock(&mutexReady);

    pthread_mutex_lock(&mutexMayorPriordad);
    if (!queue_is_empty(cola_de_mayor_prioridad))
        respuesta = 1;
    pthread_mutex_unlock(&mutexMayorPriordad);

    return respuesta;
}

int chequearRecursos(Pcb *proceso)
{
    if (procesoDelCPU->motivo == PETICION_RECURSO)
    {
        char **instruccion_separada = string_split(procesoDelCPU->instruccion, " ");

        if (!dictionary_has_key(recursos, instruccion_separada[1]))
        {
            terminarProceso(proceso, "INVALID_RESOURCE");
            string_array_destroy(instruccion_separada);
            return 1;
        }

        if (strcmp(instruccion_separada[0], "WAIT") == 0)
        {
            sem_wait(&flujoPlanificador_cp);
            proceso->estado = BLOCK;
            mensaje_cambio_de_estado("Executing", "Bloqueado", proceso->pid);
            guardar_en_cola_correspondiente_recurso(proceso, instruccion_separada);
            sem_post(&flujoPlanificador_cp);
            string_array_destroy(instruccion_separada);
            return 1;
        }
        else
        {
            hacerPOST(instruccion_separada[1], proceso->pid);
            string_array_destroy(instruccion_separada);
            return -1;
        }
        string_array_destroy(instruccion_separada);
    }
    return -1;
}

void guardar_en_cola_correspondiente_recurso(Pcb *proceso, char **instruccion_separada)
{
    Recurso *recurso = dictionary_get(recursos, instruccion_separada[1]);
    queue_push(recurso->cola_de_bloqueados_por_recurso, proceso);
    sem_post(&(recurso->procesos_en_espera));
}

void crearHilosParaWAIT()
{
    void crearHiloPorCadaRecurso(char *key, void *value)
    {
        Recurso *recurso = (Recurso *)value;
        pthread_t hilo_recurso;
        pthread_create(&hilo_recurso, NULL, waitHilo, recurso);
        pthread_detach(hilo_recurso);
    }

    dictionary_iterator(recursos, crearHiloPorCadaRecurso);
}

void *waitHilo(void *ptr)
{
    Recurso *recurso = (Recurso *)ptr;

    while (1)
    {
        sem_wait(&(recurso->procesos_en_espera));
        Pcb *proceso = (Pcb *)queue_peek(recurso->cola_de_bloqueados_por_recurso);
        recurso_wait(recurso->id_recurso, recurso->cantidad_recurso, proceso->pid, recurso->cola_de_bloqueados_por_recurso, &(recurso->mutex_recurso), &(recurso->sem_recursos));
    }

    free(params);
    pthread_exit(NULL);
}

void recurso_wait(char *id_recurso, int *cant_recurso, int pid_solicitante, t_queue *cola_de_bloqueados_recursos, pthread_mutex_t *mutex, sem_t *instancias)
{
    sem_wait(instancias);

    pthread_mutex_lock(mutex);
    *cant_recurso -= 1;
    pthread_mutex_unlock(mutex);

    sem_wait(&flujoPlanificador_cp);
    if (queue_is_empty(cola_de_bloqueados_recursos))
    {
        sem_post(instancias);
        *cant_recurso += 1;
        sem_post(&flujoPlanificador_cp);
    }
    else
    {
        Pcb *proceso = (Pcb *)queue_pop(cola_de_bloqueados_recursos);
        poner_en_lista_de_recursos_adquiridos(proceso->pid, id_recurso);
        sem_post(&flujoPlanificador_cp);
        agregarProcesoAReadyCorrespondiente(proceso);
    }
}

void poner_en_lista_de_recursos_adquiridos(int pid, char *id_recurso)
{
    Proceso_con_recurso *entrada = malloc(sizeof(Proceso_con_recurso));
    entrada->pid = pid;
    entrada->id_recurso = id_recurso;
    list_add(lista_de_procesos_con_recursos, entrada);
}

void hacerPOST(char *id_recurso, int pid)
{
    int recurso_previamente_obtenido = 0;
    bool recurso_pertenece_a_proceso(void *value)
    {
        Proceso_con_recurso *entrada = (Proceso_con_recurso *)value;
        if (entrada->pid == pid && strcmp(entrada->id_recurso, id_recurso) == 0)
        {
            return 1;
        }
        return 0;
    }

    void quitar_recurso(void *value)
    {
        Proceso_con_recurso *entrada = (Proceso_con_recurso *)value;
        recurso_previamente_obtenido = 1;
        agregar_recurso_al_diccionario(id_recurso);
        free(entrada);
    }

    list_remove_and_destroy_by_condition(lista_de_procesos_con_recursos, recurso_pertenece_a_proceso, quitar_recurso);

    if (recurso_previamente_obtenido == 0)
        agregar_recurso_al_diccionario(id_recurso);
}

void agregar_recurso_al_diccionario(char *id_recurso)
{
    Recurso *recurso = (Recurso *)dictionary_get(recursos, id_recurso);
    pthread_mutex_lock(&(recurso->mutex_recurso));
    *(recurso->cantidad_recurso) += 1;
    if (*(recurso->cantidad_recurso) > recurso->cant_recursos_iniciales)
    {
        *(recurso->cantidad_recurso) -= 1;
    }
    else
    {
        sem_post(&(recurso->sem_recursos));
    }
    pthread_mutex_unlock(&(recurso->mutex_recurso));
}

void liberar_recursos(Pcb *proceso)
{
    bool recurso_pertenece_a_proceso(void *value)
    {
        Proceso_con_recurso *entrada = (Proceso_con_recurso *)value;
        if (entrada->pid == proceso->pid)
        {
            return 1;
        }
        return 0;
    }

    void quitar_recurso(void *value)
    {
        Proceso_con_recurso *entrada = (Proceso_con_recurso *)value;
        agregar_recurso_al_diccionario(entrada->id_recurso);
        free(entrada);
    }

    list_remove_and_destroy_all_by_condition(lista_de_procesos_con_recursos, recurso_pertenece_a_proceso, quitar_recurso);
}

int chequearMotivoIO(Pcb *proceso)
{
    if (procesoDelCPU->motivo != INTERRUPCION_IO)
        return -1;

    if (verificarIOConectada(procesoDelCPU->instruccion) < 0)
    {
        terminarProceso(proceso, "INVALID_INTERFACE");
        return 1;
    }

    sem_wait(&flujoPlanificador_cp);

    procesoDelCPU->pcb->estado = BLOCK;
    mensaje_cambio_de_estado("Executing", "Bloqueado", proceso->pid);

    enviarProcesoColaIOCorrespondiente(proceso);

    sem_post(&flujoPlanificador_cp);

    return 1;
}

int verificarIOConectada(char *instruccion) // HEADER
{
    t_list *listaDeIDs = devolverKeys(interfaces_conectadas);
    char **instruccionSeparada = string_split(instruccion, " ");
    char *id_io = instruccionSeparada[1];
    int resultado = -1;

    void verificarIO(char *key, void *value)
    {
        IOguardar_kernel *io_guardada = (IOguardar_kernel *)value;

        if (strcmp(key, id_io) == 0)
        {
            // int enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion)
            if (enviarMensaje(&(io_guardada->socket), NULL, KERNEL, CHECK_CONN_IO) < 0)
            {
                list_destroy(listaDeIDs);
                quitar_interfaz(key, interfaces_conectadas);
                resultado = -1;
            }
            else if (io_guardada->interfaz == GENERICA &&
                     strcmp(instruccionSeparada[0], "IO_GEN_SLEEP") == 0)
            {
                list_destroy(listaDeIDs);
                resultado = 1;
            }
            else if (io_guardada->interfaz == STDIN &&
                     strcmp(instruccionSeparada[0], "IO_STDIN_READ") == 0)
            {
                list_destroy(listaDeIDs);
                resultado = 1;
            }
            else if (io_guardada->interfaz == STDOUT &&
                     strcmp(instruccionSeparada[0], "IO_STDOUT_WRITE") == 0)
            {
                list_destroy(listaDeIDs);
                resultado = 1;
            }
            else if (io_guardada->interfaz == DIALFS &&
                     (strcmp(instruccionSeparada[0], "IO_FS_CREATE") == 0 ||
                      strcmp(instruccionSeparada[0], "IO_FS_DELETE") == 0 ||
                      strcmp(instruccionSeparada[0], "IO_FS_TRUNCATE") == 0 ||
                      strcmp(instruccionSeparada[0], "IO_FS_WRITE") == 0 ||
                      strcmp(instruccionSeparada[0], "IO_FS_READ") == 0))
            {
                list_destroy(listaDeIDs);
                resultado = 1;
            }
        }
    }

    buscarNuevasConectadas(interfaces_conectadas, verificarIO);

    string_array_destroy(instruccionSeparada);

    return resultado;
}

int chequearMotivoExit(Pcb *proceso)
{
    if (procesoDelCPU->motivo != EXIT_SIGNAL && procesoDelCPU->motivo != INTERRUPCION_EXIT_KERNEL && procesoDelCPU->motivo != OUT_OF_MEMORY)
        return -1;

    char *motivo_exit = enum_to_string_EXIT(procesoDelCPU->motivo);

    terminarProceso(proceso, motivo_exit);
    return 1;
}

char *enum_to_string_EXIT(MotivoDesalojo motivo)
{
    if (motivo == EXIT_SIGNAL)
        return "SUCCESS";
    if (motivo == INTERRUPCION_EXIT_KERNEL)
        return "INTERRUPTED_BY_USER";
    if (motivo == OUT_OF_MEMORY)
        return "OUT_OF_MEMORY";
}

void esperarProcesoCPU(int quantum)
{
    // Creo el hilo para escuchar a CPU
    pthread_t hiloEscuchaCPUDispatch;
    pthread_create(&hiloEscuchaCPUDispatch, NULL, escuchaDispatch, (void *)params->KernelSocketCPUDispatch);

    // Si no devulve nada en ciertos segundos mando interrupcion por fin de quantum
    if (algoritmo == RR || algoritmo == VRR)
    {
        int tiempoRestante = esperarQuantum(quantum);
        temporal_destroy(tiempoEjecutando);

        sem_wait(&esperar_proceso);

        if (algoritmo == VRR && PIDprocesoEjecutando < 0)
        {
            procesoDelCPU->pcb->quantumRestante = tiempoRestante;
        }
        else if (PIDprocesoEjecutando < 0)
        {
            procesoDelCPU->pcb->quantumRestante = 0;
        }
    }
    else
    {
        procesoDelCPU->pcb->quantumRestante = 0;
    }

    pthread_join(hiloEscuchaCPUDispatch, NULL);
    sem_post(&esperar_guardar_proceso);
}

int esperarQuantum(int quantum)
{
    tiempoEjecutando = temporal_create();

    while (1)
    {
        if (PIDprocesoEjecutando < 0)
        {
            return (quantum - temporal_gettime(tiempoEjecutando));
        }
        else if (temporal_gettime(tiempoEjecutando) >= quantum)
        {
            enviarInterrupcion(FIN_DE_QUANNTUM, &(params->KernelSocketCPUInterrumpt));
            return 0;
        }
    }
}

void enviarInterrupcion(MotivoDesalojo motivo, int *socket)
{
    t_buffer *buffer = buffer_create(4);

    switch (motivo)
    {
    case FIN_DE_QUANNTUM:
        buffer_add_uint32(buffer, 2);
        break;

    case INTERRUPCION_EXIT_KERNEL:
        buffer_add_uint32(buffer, 1);
        break;

    default:
        break;
    }

    enviarMensaje(socket, buffer, KERNEL, MENSAJE);
}

void *escuchaDispatch(void *ptr)
{
    TipoModulo *modulo = get_modulo_msg_recv(&(params->KernelSocketCPUDispatch));
    op_code *op_code = get_opcode_msg_recv(&(params->KernelSocketCPUDispatch));

    t_buffer *buffer = buffer_leer_recv(&(params->KernelSocketCPUDispatch));

    procesoDelCPU->motivo = buffer_read_uint32(buffer);
    procesoDelCPU->pcb->pid = buffer_read_uint32(buffer);
    procesoDelCPU->pcb->pc = buffer_read_uint32(buffer);
    obtener_registros_pcbCPU(buffer, procesoDelCPU);
    procesoDelCPU->pcb->SI = buffer_read_uint32(buffer);
    procesoDelCPU->pcb->DI = buffer_read_uint32(buffer);

    if (procesoDelCPU->motivo == INTERRUPCION_IO || procesoDelCPU->motivo == PETICION_RECURSO)
    {
        int len = buffer_read_uint32(buffer);
        procesoDelCPU->instruccion = buffer_read_string(buffer, len);
    }

    PIDprocesoEjecutando = -1; // llego el proceso, ya no esta ejecutando ninguno
    sem_post(&esperar_proceso);

    buffer_destroy(buffer);
    free(modulo);
    free(op_code);
    pthread_exit(NULL);
}

void obtener_registros_pcbCPU(t_buffer *buffer, MensajeProcesoDelCPU *proceso)
{
    proceso->pcb->registros.ax.u8 = buffer_read_uint8(buffer);
    proceso->pcb->registros.eax.i32 = buffer_read_uint32(buffer);
    proceso->pcb->registros.bx.u8 = buffer_read_uint8(buffer);
    proceso->pcb->registros.ebx.i32 = buffer_read_uint32(buffer);
    proceso->pcb->registros.cx.u8 = buffer_read_uint8(buffer);
    proceso->pcb->registros.ecx.i32 = buffer_read_uint32(buffer);
    proceso->pcb->registros.dx.u8 = buffer_read_uint8(buffer);
    proceso->pcb->registros.edx.i32 = buffer_read_uint32(buffer);
}

void enviarProcesoColaIOCorrespondiente(Pcb *proceso)
{
    char **instruccionSeparada = string_split(procesoDelCPU->instruccion, " ");
    proceso->estado = BLOCK;

    structGuardarProcesoEnBloqueado *procesoInstruccion = (structGuardarProcesoEnBloqueado *)malloc(sizeof(structGuardarProcesoEnBloqueado));

    procesoInstruccion->instruccion = procesoDelCPU->instruccion;
    procesoInstruccion->procesoPCB = proceso;

    if (strcmp(instruccionSeparada[0], "IO_GEN_SLEEP") == 0)
    {
        guardarEnColaGenerico(procesoInstruccion);
    }
    else if (strcmp(instruccionSeparada[0], "IO_STDIN_READ") == 0)
    {
        guardarEnColaSTDIN(procesoInstruccion);
    }
    else if (strcmp(instruccionSeparada[0], "IO_STDOUT_WRITE") == 0)
    {
        guardarEnColaSTDOUT(procesoInstruccion);
    }
    else
    {
        guardarEnColaDIALFS(procesoInstruccion);
    }

    string_array_destroy(instruccionSeparada);
}

void guardarEnColaGenerico(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIOGenerico);

    queue_push(lista_bloqueados_generico, proceso);

    sem_post(&esperando_generico_sem);

    pthread_mutex_unlock(&mutexIOGenerico);
}

void guardarEnColaSTDIN(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIOSTDIN);

    queue_push(lista_bloqueados_STDIN, proceso);

    sem_post(&esperando_stdin_sem);

    pthread_mutex_unlock(&mutexIOSTDIN);
}

void guardarEnColaSTDOUT(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIOSTDOUT);

    queue_push(lista_bloqueados_STDOUT, proceso);

    sem_post(&esperando_stdout_sem);

    pthread_mutex_unlock(&mutexIOSTDOUT);
}

void guardarEnColaDIALFS(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIODIALFS);

    queue_push(lista_bloqueados_DialFS, proceso);

    sem_post(&esperando_dialfs_sem);

    pthread_mutex_unlock(&mutexIODIALFS);
}

void *manageBloqueados(void *ptr) // iniciar hilo en el planificador
{
    (void)ptr;

    pthread_t hiloBloqueadosGenericos;
    pthread_t hiloBloqueadosSTDIN;
    pthread_t hiloBloqueadosSTDOUT;
    pthread_t hiloBloqueadosDIALFS;

    pthread_create(&hiloBloqueadosGenericos, NULL, manageIO_Kernel, (void *)GENERICA);
    pthread_create(&hiloBloqueadosSTDIN, NULL, manageIO_Kernel, (void *)STDIN);
    pthread_create(&hiloBloqueadosSTDOUT, NULL, manageIO_Kernel, (void *)STDOUT);
    pthread_create(&hiloBloqueadosDIALFS, NULL, manageIO_Kernel, (void *)DIALFS);

    pthread_detach(hiloBloqueadosGenericos);
    pthread_detach(hiloBloqueadosSTDIN);
    pthread_detach(hiloBloqueadosSTDOUT);
    pthread_detach(hiloBloqueadosDIALFS);

    pthread_exit(NULL);
}

void *manageIO_Kernel(void *ptr)
{
    TipoInterfaz tipo_interfaz = (TipoInterfaz)ptr;
    t_list *identificadoresIOConectadas;
    t_list *listasPorCadaID;
    listasPorCadaID = list_create();

    pthread_mutex_lock(&mutexListaTodosPorIDio);
    list_add(todas_las_listasBloqueadosPorIDio, listasPorCadaID);
    pthread_mutex_unlock(&mutexListaTodosPorIDio);

    structGuardarProcesoEnBloqueado *proceso;

    t_queue *lista_bloqueados = obtenerColaCorrespondiente(tipo_interfaz);
    sem_t *sem_hay_procesos_esperando = obtenerSemaforoCorrespondiente(tipo_interfaz);

    while (1)
    {

        sem_wait(sem_hay_procesos_esperando);

        buscar_ios_conectadas();

        identificadoresIOConectadas = dictionary_keys(interfaces_conectadas);
        // obtenerKeys(identificadoresIOConectadas);
        ordenarListaConLasIOsConectadas(tipo_interfaz, listasPorCadaID); // HACE UN 'dictionary_iterator' con 'interfaces_conectadas'

        sem_wait(&flujoPlanificador_cp);
        proceso = (structGuardarProcesoEnBloqueado *)queue_pop(lista_bloqueados);
        sem_post(&flujoPlanificador_cp);

        if (laIOEstaConectada(identificadoresIOConectadas, proceso) > 0)
        {
            guardarEnSuCola(listasPorCadaID, proceso);
        }
        else
        {
            terminarProceso(proceso->procesoPCB, "INVALID_INTERFACE");
        }

        for (int i = 0; i < list_size(listasPorCadaID); i++)
        {
            structParaHiloFORIO *params = malloc(sizeof(structParaHiloFORIO));
            params->lista = list_get(listasPorCadaID, i);
            params->interfaz = tipo_interfaz;
            pthread_t hiloParaID;
            pthread_create(&hiloParaID, NULL, manageGenericoPorID, (void *)params);
            pthread_detach(hiloParaID);
        }
        /*
             Primero guarda las io (en este caso genericas) conectadas junto a una cola de procesos
            luego se determina si la io que pide el proceso esta efectivamente conectada y se guarda
            en la cola correspondiente, si no es asi se termina el proceso por ERROR DE IO.
             Luego se crea un hilo por cada io en donde se manejan sus colas. Adentro de la funcion del hilo
            se verifica que la entradasalida siga conectada, como existiran muchos flujos de ejecucion que interactuen
            con la misma se utilizan semaforos binarios que estaran guardados en 'listasPorCadaID'. Una vez recibida la finalizacion
            del io se sacara de la cola de bloqueado y se guardara en Ready o MayorPrioridad o se mandara a EXIT segun corresponda.
        */
        list_destroy(identificadoresIOConectadas);
    }
}

void buscar_ios_conectadas()
{
    char **keys = string_array_new();

    void verificar_desconecada(char *key, void *value)
    {
        IOguardar_kernel *io_guardada = (IOguardar_kernel *)value;
        // int enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion)
        if (enviarMensaje(&(io_guardada->socket), NULL, KERNEL, CHECK_CONN_IO) < 0)
        {
            string_array_push(&keys, key);
        }
    }

    dictionary_iterator(interfaces_conectadas, verificar_desconecada);

    for (int i = 0; i < string_array_size(keys); i++)
    {
        quitar_interfaz(keys[i], interfaces_conectadas);
    }

    string_array_destroy(keys);
}

sem_t *obtenerSemaforoCorrespondiente(TipoInterfaz interfaz) // HEADER
{
    switch (interfaz)
    {
    case GENERICA:
        return &esperando_generico_sem;
        break;

    case STDIN:
        return &esperando_stdin_sem;
        break;

    case STDOUT:
        return &esperando_stdout_sem;
        break;

    case DIALFS:
        return &esperando_dialfs_sem;
        break;

    default:
        break;
    }
}

int laIOEstaConectada(t_list *conectadas, structGuardarProcesoEnBloqueado *proceso)
{
    int estaConectada = -1;
    char *keyProceso = string_split(proceso->instruccion, " ")[1];

    void saberSiEstaConectada(void *value)
    {
        char *key = (char *)value;
        if (strcmp(key, keyProceso) == 0)
            estaConectada = 1;
    }

    list_iterate(conectadas, saberSiEstaConectada);

    return estaConectada;
}

void guardarEnSuCola(t_list *listasPorCadaID, structGuardarProcesoEnBloqueado *proceso)
{
    char **instruccionSeparada = string_split(proceso->instruccion, " ");
    char *ID_IOProceso = instruccionSeparada[1];

    void guardarProcesoYInstruccion(void *value)
    {
        listaBlockPorID *io_conectada_por_id = (listaBlockPorID *)value;
        if (strcmp(io_conectada_por_id->identificador, ID_IOProceso) == 0)
        {
            sem_wait(&flujoPlanificador_cp);
            pthread_mutex_lock(&io_conectada_por_id->mutexCola);
            queue_push(io_conectada_por_id->colaBloqueadoPorID, proceso);
            pthread_mutex_unlock(&io_conectada_por_id->mutexCola);
            sem_post(&flujoPlanificador_cp);
        }
    }

    list_iterate(listasPorCadaID, guardarProcesoYInstruccion);

    string_array_destroy(instruccionSeparada);
}

t_queue *obtenerColaCorrespondiente(TipoInterfaz tipo_interfaz)
{
    switch (tipo_interfaz)
    {
    case GENERICA:
        return lista_bloqueados_generico;
        break;

    case STDIN:
        return lista_bloqueados_STDIN;
        break;

    case STDOUT:
        return lista_bloqueados_STDOUT;
        break;

    case DIALFS:
        return lista_bloqueados_DialFS;
        break;

    default:
        break;
    }
}

void obtenerKeys(t_list *identificadoresIOConectadas)
{
    identificadoresIOConectadas = dictionary_keys(interfaces_conectadas); // poner funcion en Interfaces
}

t_list *ordenarListaConLasIOsConectadas(TipoInterfaz tipo, t_list *listasPorCadaID)
{
    void encontrarIONuevaConectada(char *key, void *value)
    {
        PonerIO(key, value, listasPorCadaID, tipo); // pone la IO en listasPorCadaID si no existe
    }

    buscarNuevasConectadas(interfaces_conectadas, encontrarIONuevaConectada);

    MarcarDesconetadas(listasPorCadaID);
}

void PonerIO(char *key, void *value, t_list *listasPorCadaID, TipoInterfaz tipo)
{
    IOguardar_kernel *io_a_guardar = (IOguardar_kernel *)value;

    if (io_a_guardar->interfaz == tipo)
    {
        if (estaEnLaLista(key, listasPorCadaID) < 0) // HACER LA FUNCION
        {
            listaBlockPorID *io_lista = (listaBlockPorID *)malloc(sizeof(listaBlockPorID));
            io_lista->conectado = malloc(4);
            t_queue *io_queue = queue_create();
            io_lista->colaBloqueadoPorID = io_queue;
            *(io_lista->conectado) = 1;
            io_lista->identificador = key;
            io_lista->socket = io_a_guardar->socket;
            sem_init(&(io_lista->semEsperarBlock), 0, 1);
            pthread_mutex_init(&(io_lista->mutexCola), NULL);
            list_add(listasPorCadaID, io_lista);
        }
    }
}

int estaEnLaLista(char *key, t_list *lista)
{
    int resultado = -1;

    void encontrar(void *value)
    {
        listaBlockPorID *dato = (listaBlockPorID *)value;
        if (strcmp(dato->identificador, key) == 0)
            resultado = 1;
    }

    list_iterate(lista, encontrar);

    return resultado;
}

void MarcarDesconetadas(t_list *listasPorCadaID)
{
    void verificarIOConectada(void *value)
    {
        listaBlockPorID *io = (listaBlockPorID *)value;
        if (!dictionary_has_key(interfaces_conectadas, io->identificador)) // poner mutex en Interfaces
        {
            *(io->conectado) = -1;
        }
    }

    list_iterate(listasPorCadaID, verificarIOConectada);
}

void *manageGenericoPorID(void *ptr)
{
    structParaHiloFORIO *params = (structParaHiloFORIO *)ptr;
    listaBlockPorID *cola = params->lista;

    if (*(cola->conectado) < 0)
    {
        free(params);
        pthread_exit(NULL);
    }

    sem_wait(&cola->semEsperarBlock);

    if (queue_is_empty(cola->colaBloqueadoPorID)) // poner un mutex para esto por las dudas
    {
        sem_post(&cola->semEsperarBlock);
        free(params);
        pthread_exit(NULL);
    }

    structGuardarProcesoEnBloqueado *proceso;

    if (!queue_is_empty(cola->colaBloqueadoPorID))
    {
        proceso = (structGuardarProcesoEnBloqueado *)queue_peek(cola->colaBloqueadoPorID); // TIENE QUE AGARRAR EL PCB JUNTO A LA INSTRUCCION
    }
    else
    {
        sem_post(&cola->semEsperarBlock);
        free(params);
        pthread_exit(NULL);
    }

    int pid = proceso->procesoPCB->pid;

    enviarMensajeAInterfaz(proceso, &(cola->socket), params->interfaz);

    int result = esperarConfirmacion(&(cola->socket));

    sem_wait(&flujoPlanificador_cp);
    if (!queue_is_empty(cola->colaBloqueadoPorID))
    {
        structGuardarProcesoEnBloqueado *proceso_estado = (structGuardarProcesoEnBloqueado *)queue_peek(cola->colaBloqueadoPorID);
        if (proceso_estado->procesoPCB->pid == pid)
        {
            quitarProcesoDeLaCola(result, cola, proceso->procesoPCB->pid);
        }
    }
    else
    {
        sem_post(&cola->semEsperarBlock);
        sem_post(&flujoPlanificador_cp);
        free(params);
        pthread_exit(NULL);
    }

    sem_post(&cola->semEsperarBlock);

    pthread_exit(NULL);
}

void enviarMensajeAInterfaz(structGuardarProcesoEnBloqueado *proceso, int *socket, TipoInterfaz tipo) // ENVIAR LO NECESARIO PARA TODAS LAS IO
{
    t_buffer *buffer = buffer_create(strlen(proceso->instruccion) + 1 + 4 + 4);
    buffer_add_uint32(buffer, proceso->procesoPCB->pid);
    buffer_add_string(buffer, strlen(proceso->instruccion) + 1, proceso->instruccion);
    free(proceso->instruccion);
    enviarMensaje(socket, buffer, KERNEL, MENSAJE);
}

int esperarConfirmacion(int *socket)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *codigo = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);

    int resultado = buffer_read_uint32(buffer);

    buffer_destroy(buffer);

    return resultado;
}

void quitarProcesoDeLaCola(int resultadoDeLaIO, listaBlockPorID *cola, int pid)
{
    pthread_mutex_lock(&(cola->mutexCola));
    structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)queue_peek(cola->colaBloqueadoPorID);
    if (proceso->procesoPCB->pid == pid)
    {
        structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)queue_pop(cola->colaBloqueadoPorID);
        sem_post(&flujoPlanificador_cp);
        pthread_mutex_unlock(&(cola->mutexCola));

        if (resultadoDeLaIO < 0)
            terminarProceso(proceso->procesoPCB, "ERROR_DE_IO");
        else
            agregarProcesoAReadyCorrespondiente(proceso->procesoPCB);

        free(proceso);
    }
    else
    {
        sem_post(&flujoPlanificador_cp);
        pthread_mutex_unlock(&(cola->mutexCola));
    }
}

int hayProcesosPrioritarios()
{
    int resultado = 1;

    pthread_mutex_lock(&mutexMayorPriordad);

    if (queue_is_empty(cola_de_mayor_prioridad))
        resultado = -1;

    pthread_mutex_unlock(&mutexMayorPriordad);

    return resultado;
}

int enviarProcesoMayorPrioridadCPU()
{
    pthread_mutex_lock(&mutexMayorPriordad);
    Pcb *proceso = (Pcb *)queue_pop(cola_de_mayor_prioridad);
    pthread_mutex_unlock(&mutexMayorPriordad);

    PIDprocesoEjecutando = proceso->pid;

    int quantum = proceso->quantumRestante;

    enviarMensajeCPUPCBProceso(proceso); // Mandar solo pcb. En la cola de ready guardar la pcb solamente

    return quantum;
}

void enviarProcesoReadyCPU()
{
    sem_wait(&flujoPlanificador_cp);

    pthread_mutex_lock(&mutexReady);
    Pcb *proceso = (Pcb *)queue_pop(cola_de_ready); // segmentation foult
    pthread_mutex_unlock(&mutexReady);

    PIDprocesoEjecutando = proceso->pid;

    enviarMensajeCPUPCBProceso(proceso); // Mandar solo pcb. En la cola de ready guardar la pcb solamente

    sem_post(&flujoPlanificador_cp);
}

void enviarMensajeCPUPCBProceso(Pcb *proceso)
{
    t_buffer *buffer = buffer_create(sizeof(Pcb));
    buffer_add_uint32(buffer, proceso->pid);
    buffer_add_uint32(buffer, proceso->pc);
    agregarRegistrosAlBuffer(buffer, proceso);
    buffer_add_uint32(buffer, proceso->SI);
    buffer_add_uint32(buffer, proceso->DI);

    proceso->estado = EXEC;
    mensaje_cambio_de_estado("Ready", "Executing", proceso->pid);

    enviarMensaje(&(params->KernelSocketCPUDispatch), buffer, KERNEL, MENSAJE);

    free(proceso);
}

void agregarRegistrosAlBuffer(t_buffer *buffer, Pcb *proceso)
{
    buffer_add_uint8(buffer, proceso->registros.ax.u8);
    buffer_add_uint32(buffer, proceso->registros.eax.i32);
    buffer_add_uint8(buffer, proceso->registros.bx.u8);
    buffer_add_uint32(buffer, proceso->registros.ebx.i32);
    buffer_add_uint8(buffer, proceso->registros.cx.u8);
    buffer_add_uint32(buffer, proceso->registros.ecx.i32);
    buffer_add_uint8(buffer, proceso->registros.dx.u8);
    buffer_add_uint32(buffer, proceso->registros.edx.i32);
}

int hayAlgunoEnCPU()
{
    return PIDprocesoEjecutando;
}

void agregarProcesoAReadyCorrespondiente(Pcb *proceso)
{
    sem_wait(&flujoPlanificador_cp);

    if (algoritmo == VRR && proceso->quantumRestante > 0)
        agregarProcesoColaMayorPrioridad(proceso);
    else
        agregarProcesoColaReady(proceso);

    char *estado_previo = enum_to_string(proceso->estado);

    proceso->estado = READY;
    mensaje_cambio_de_estado(estado_previo, "Ready", proceso->pid);

    sem_post(&flujoPlanificador_cp);
}

char *enum_to_string(EstadoProceso estado) // HEADER
{
    if (estado == EXEC)
        return "Executing";
    if (estado == BLOCK)
        return "Bloqueado";
    if (estado == NEW)
        return "New";
    return "NULL";
}

void mensaje_desalojo()
{
    char *mensaje = string_new();

    if (procesoDelCPU->motivo == FIN_DE_QUANNTUM)
    {
        char *pid = string_itoa(procesoDelCPU->pcb->pid);

        string_append(&mensaje, "PID: ");
        string_append(&mensaje, pid);
        string_append(&mensaje, " - Desalojado por fin de Quantum");

        log_info(kernel_loger_cp, mensaje);
    }
    else if (procesoDelCPU->motivo == PETICION_RECURSO || procesoDelCPU->motivo == INTERRUPCION_IO)
    {
        char **instruccionSeparada = string_split(procesoDelCPU->instruccion, " ");
        char *recurso = instruccionSeparada[1];

        string_append(&mensaje, "PID: ");
        string_append(&mensaje, string_itoa(procesoDelCPU->pcb->pid));
        string_append(&mensaje, " - Bloqueado por: ");
        string_append(&mensaje, recurso);

        log_info(kernel_loger_cp, mensaje);
        string_array_destroy(instruccionSeparada);
    }

    free(mensaje);
}

void mensaje_cambio_de_estado(char *estado_anterior, char *estado_siguiente, int pid)
{
    char *mensaje = string_new();

    string_append(&mensaje, "PID: ");
    string_append(&mensaje, string_itoa(pid));
    string_append(&mensaje, " - Estado anterior: ");
    string_append(&mensaje, estado_anterior);
    string_append(&mensaje, " - Estado actual: ");
    string_append(&mensaje, estado_siguiente);

    log_info(kernel_loger_cp, mensaje);

    free(mensaje);
}

void guardarLosRegistros(Pcb *proceso)
{
    proceso->registros.ax.u8 = procesoDelCPU->pcb->registros.ax.u8;
    proceso->registros.eax.i32 = procesoDelCPU->pcb->registros.eax.i32;
    proceso->registros.bx.u8 = procesoDelCPU->pcb->registros.bx.u8;
    proceso->registros.ebx.i32 = procesoDelCPU->pcb->registros.ebx.i32;
    proceso->registros.cx.u8 = procesoDelCPU->pcb->registros.cx.u8;
    proceso->registros.ecx.i32 = procesoDelCPU->pcb->registros.ecx.i32;
    proceso->registros.dx.u8 = procesoDelCPU->pcb->registros.dx.u8;
    proceso->registros.edx.i32 = procesoDelCPU->pcb->registros.edx.i32;
}

void agregarProcesoColaMayorPrioridad(Pcb *procesoPCB)
{
    pthread_mutex_lock(&mutexMayorPriordad);

    queue_push(cola_de_mayor_prioridad, procesoPCB);

    sem_post(&cant_procesos_ready);

    log_ingreso_a_ready("Ready Prioridad", procesoPCB);

    pthread_mutex_unlock(&mutexMayorPriordad);
}

void agregarProcesoColaReady(Pcb *procesoPCB)
{
    pthread_mutex_lock(&mutexReady);

    queue_push(cola_de_ready, procesoPCB);

    sem_post(&cant_procesos_ready);

    log_ingreso_a_ready("Cola Ready", procesoPCB);

    pthread_mutex_unlock(&mutexReady);
}

void log_ingreso_a_ready(char *cola, Pcb *proceso_nuevo)
{
    char *array_pids = obtener_array_de_pids(cola, proceso_nuevo);

    char *mensaje = string_new();

    string_append(&mensaje, cola);
    string_append(&mensaje, ": ");
    string_append(&mensaje, array_pids);

    log_info(kernel_loger_cp, mensaje);

    free(mensaje);
    free(array_pids);
}

char *obtener_array_de_pids(char *cola_c, Pcb *proceso_nuevo)
{
    t_queue *cola;

    char *array = string_new();

    if (strcmp(cola_c, "Cola Ready") == 0)
        cola = cola_de_ready;
    else
        cola = cola_de_mayor_prioridad;

    string_append(&array, "[ ");

    void iterar_procesos(void *value)
    {
        Pcb *proceso = (Pcb *)value;

        string_append(&array, string_itoa(proceso->pid));

        if (proceso->pid != proceso_nuevo->pid)
            string_append(&array, ", ");
    }

    list_iterate(cola->elements, iterar_procesos);

    string_append(&array, " ]");

    return array;
}

void detenerPlanificador()
{
    sem_wait(&flujoPlanificador_cp);
}

void reanudarPlanificador()
{
    sem_post(&flujoPlanificador_cp);
}

int encontrar_y_terminar_proceso(int pid)
{
    /*
        Para hacer que la funcion tenga el correcto comportamiento
        es necesario detener la planificacion del sistema.
    */

    if (PIDprocesoEjecutando == pid)
    {
        interrumpir_ejecucion();
        return pid;
    }

    if (buscar_colas_recursos_terminar(pid) > 0)
        return pid;

    if (buscar_en_bloqueados_y_terminar(pid) > 0)
        return pid;

    if (buscar_en_readys(pid) > 0)
        return pid;

    return -1;
}

int buscar_en_readys(int pid)
{
    Pcb *proceso = NULL;

    bool encontrar_proceso(void *value)
    {
        Pcb *proceso = (Pcb *)value;
        if (proceso->pid == pid)
            return 1;
        return 0;
    }

    proceso = list_remove_by_condition(cola_de_mayor_prioridad->elements, encontrar_proceso);
    if (proceso != NULL)
    {
        terminarProceso(proceso, "INTERRUPTED_BY_USER");
        return 1;
    }
    proceso = list_remove_by_condition(cola_de_ready->elements, encontrar_proceso);
    if (proceso != NULL)
    {
        terminarProceso(proceso, "INTERRUPTED_BY_USER");
        return 1;
    }
    return -1;
}

int buscar_en_bloqueados_y_terminar(int pid)
{
    structGuardarProcesoEnBloqueado *proceso;

    bool encontrar_proceso_bloqueado(void *value)
    {
        structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)value;
        if (proceso->procesoPCB->pid == pid)
            return 1;
        return 0;
    }

    proceso = (structGuardarProcesoEnBloqueado *)list_remove_by_condition(lista_bloqueados_generico->elements, encontrar_proceso_bloqueado);
    if (proceso != NULL)
    {
        terminarProceso(proceso->procesoPCB, "INTERRUPTED_BY_USER");
        free(proceso);
        return 1;
    }

    proceso = (structGuardarProcesoEnBloqueado *)list_remove_by_condition(lista_bloqueados_STDIN->elements, encontrar_proceso_bloqueado);
    if (proceso != NULL)
    {
        terminarProceso(proceso->procesoPCB, "INTERRUPTED_BY_USER");
        free(proceso);
        return 1;
    }

    proceso = (structGuardarProcesoEnBloqueado *)list_remove_by_condition(lista_bloqueados_STDOUT->elements, encontrar_proceso_bloqueado);
    if (proceso != NULL)
    {
        terminarProceso(proceso->procesoPCB, "INTERRUPTED_BY_USER");
        free(proceso);
        return 1;
    }

    proceso = (structGuardarProcesoEnBloqueado *)list_remove_by_condition(lista_bloqueados_DialFS->elements, encontrar_proceso_bloqueado);
    if (proceso != NULL)
    {
        terminarProceso(proceso->procesoPCB, "INTERRUPTED_BY_USER");
        free(proceso);
        return 1;
    }

    return buscar_bloqueados_porIDio(pid);
}

int buscar_bloqueados_porIDio(int pid)
{
    int encontro_y_termino_proceso = -1;
    bool encontrar_proceso_y_terminarlo(void *value)
    {
        structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)value;
        if (proceso->procesoPCB->pid == pid)
        {
            terminarProceso(proceso->procesoPCB, "INTERRUPTED_BY_USER");
            encontro_y_termino_proceso = 1;
            return 1;
        }
        return 0;
    }

    void iterar_sobre_colas_de_espera_por_cada_io(void *value)
    {
        listaBlockPorID *io_lista = (listaBlockPorID *)value;
        // list_iterate(io_lista->colaBloqueadoPorID->elements, encontrar)
        structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)list_remove_by_condition(io_lista->colaBloqueadoPorID->elements, encontrar_proceso_y_terminarlo);
        free(proceso);
    }

    void iterar_sobre_todas_las_listas(void *value)
    {
        t_list *listasPorCadaID = (t_list *)value;
        list_iterate(listasPorCadaID, iterar_sobre_colas_de_espera_por_cada_io);
    }

    list_iterate(todas_las_listasBloqueadosPorIDio, iterar_sobre_todas_las_listas);

    return encontro_y_termino_proceso;
}

int buscar_colas_recursos_terminar(int pid)
{
    int resultado = -1;
    void buscar_proceso_por_recurso(char *key, void *value)
    {
        Recurso *recurso = (Recurso *)value;
        buscar_en_espera(recurso->cola_de_bloqueados_por_recurso, pid, &resultado, &(recurso->mutex_recurso));
    }

    dictionary_iterator(recursos, buscar_proceso_por_recurso);

    return resultado;
}

void buscar_en_espera(t_queue *cola, int pid, int *resultado, pthread_mutex_t *mutex)
{
    Pcb *proceso = NULL;

    bool encontrar_proceso(void *value)
    {
        Pcb *proceso = (Pcb *)value;
        if (proceso->pid == pid)
            return 1;
        return 0;
    }

    pthread_mutex_lock(mutex);
    proceso = (Pcb *)list_remove_by_condition(cola->elements, encontrar_proceso);
    pthread_mutex_unlock(mutex);

    if (proceso != NULL)
    {
        terminarProceso(proceso, "INTERRUPTED_BY_USER");
        *resultado = 1;
    }
}

void interrumpir_ejecucion()
{
    enviarInterrupcion(INTERRUPCION_EXIT_KERNEL, &(params->KernelSocketCPUInterrumpt));
}

void listar_por_estado()
{
    t_log *loger_estados_cp = log_create("logs/kernel_info.log", "plani_cp", 1, LOG_LEVEL_INFO);
    char *mensaje_cp_readys = string_new();
    string_append(&mensaje_cp_readys, "READY [ ");
    void recorrer(void *value)
    {
        Pcb *proceso = (Pcb *)value;
        string_append(&mensaje_cp_readys, string_itoa(proceso->pid));
        string_append(&mensaje_cp_readys, ", ");
    }
    list_iterate(cola_de_ready->elements, recorrer);
    list_iterate(cola_de_mayor_prioridad->elements, recorrer);
    string_append(&mensaje_cp_readys, "]");
    log_info(loger_estados_cp, mensaje_cp_readys);
    free(mensaje_cp_readys);

    char *mensaje_cp_bloqueado = string_new();
    string_append(&mensaje_cp_bloqueado, "BLOQUEADOS [ ");

    void iterar_cola(void *value) // NOOOOO
    {
        structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)value;
        string_append(&mensaje_cp_bloqueado, string_itoa(proceso->procesoPCB->pid));
        string_append(&mensaje_cp_bloqueado, ", ");
    }

    void iterar_sobre_colas_de_espera_por_cada_io(void *value)
    {
        listaBlockPorID *io_lista = (listaBlockPorID *)value;
        // list_iterate(io_lista->colaBloqueadoPorID->elements, encontrar)
        list_iterate(io_lista->colaBloqueadoPorID->elements, iterar_cola);
    }

    void iterar_sobre_todas_las_listas(void *value)
    {
        t_list *listasPorCadaID = (t_list *)value;
        list_iterate(listasPorCadaID, iterar_sobre_colas_de_espera_por_cada_io);
    }

    list_iterate(todas_las_listasBloqueadosPorIDio, iterar_sobre_todas_las_listas);

    void iterar_procesos_recurso(void *value)
    {
        Pcb *proceso = (Pcb *)value;
        string_append(&mensaje_cp_bloqueado, string_itoa(proceso->pid));
        string_append(&mensaje_cp_bloqueado, ", ");
    }

    void buscar_proceso_por_recurso(char *key, void *value)
    {
        Recurso *recurso = (Recurso *)value;
        list_iterate(recurso->cola_de_bloqueados_por_recurso->elements, iterar_procesos_recurso);
    }

    dictionary_iterator(recursos, buscar_proceso_por_recurso);

    string_append(&mensaje_cp_bloqueado, "]");
    log_info(loger_estados_cp, mensaje_cp_bloqueado);
    free(mensaje_cp_bloqueado);

    log_destroy(loger_estados_cp);
}