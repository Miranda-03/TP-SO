#include "planificadorCP.h"

ParamsPCP_kernel *params;
int *PIDprocesoEjecutando;
t_temporal *tiempoEjecutando;
int *quantumTotal;
int *quantumRestante;
// int socketBidireccionalMemoria_Kernel;
Algoritmo algoritmo;

Pcb* proceosPCB_HILO_recursos; // ESTO SE PUEDE MEJORAR

t_queue *cola_de_ready;
t_queue *cola_de_mayor_prioridad;
t_queue *lista_bloqueados_generico;
t_queue *lista_bloqueados_STDIN;
t_queue *lista_bloqueados_STDOUT;
t_queue *lista_bloqueados_DialFS;

t_dictionary *interfaces_conectadas;
t_dictionary *recursos;

pthread_mutex_t mutexReady;
pthread_mutex_t mutexMayorPriordad;
pthread_mutex_t mutexIOGenerico;
pthread_mutex_t mutexIOSTDIN;
pthread_mutex_t mutexIOSTDOUT;

/*
    PONER LOS PROTOTIPOS DE LAS FUNCIONES EN EL HEADER IOguardar
*/

// CREO QUE HAY QUE INICIALIZAR EL 'procesoDelCPU' PARA DESPUES PODER USARLO
MensajeProcesoDelCPU *procesoDelCPU;

void *planificarCortoPlazo(void *ptr)
{
    params = (ParamsPCP_kernel *)ptr;
    interfaces_conectadas = params->interfaces_conectadas;
    recursos = params->recursos;
    algoritmo = params->algoritmo;
    *PIDprocesoEjecutando = -1;
    *quantumTotal = atoi(obtenerValorConfig(PATH_CONFIG, "QUANTUM"));
    *quantumRestante = *quantumTotal;

    cola_de_ready = queue_create();
    cola_de_mayor_prioridad = queue_create();
    lista_bloqueados_generico = queue_create();
    lista_bloqueados_STDIN = queue_create();
    lista_bloqueados_STDOUT = queue_create();
    lista_bloqueados_DialFS = queue_create();

    pthread_mutex_init(&mutexReady, NULL);
    pthread_mutex_init(&mutexIOGenerico, NULL);
    pthread_mutex_init(&mutexIOSTDIN, NULL);
    pthread_mutex_init(&mutexIOSTDOUT, NULL);

    pthread_t hiloParaBloqueados;
    pthread_create(&hiloParaBloqueados, NULL, manageBloqueados, NULL);
    pthread_detach(hiloParaBloqueados);

    while (1)
    {
        if (hayAlgunoEnCPU() < 0)
        {
            if (algoritmo == VRR)
            {
                if (hayProcesosPrioritarios() > 0)
                {
                    *quantumRestante = enviarProcesoMayorPrioridadCPU();
                }
                else
                {
                    *quantumRestante = *quantumTotal;
                    enviarProcesoReadyCPU();
                }
                // AGREGAR COLA DE MAYOR PRIORIDAD
            }
            else
            {
                *quantumRestante = *quantumTotal;
                enviarProcesoReadyCPU();
            }

            esperarProcesoCPU(*quantumRestante);

            // Crear el proceso (PCB) con un malloc
            Pcb *procesoPCB = malloc(sizeof(Pcb));
            procesoPCB->pc = procesoDelCPU->pcb->pc;
            procesoPCB->pid = procesoDelCPU->pcb->pid;
            procesoPCB->quantumRestante = procesoDelCPU->pcb->quantumRestante;
            guardarLosRegistros(procesoPCB);

            proceosPCB_HILO_recursos = procesoPCB;

            if (chequearMotivoIO(procesoPCB) < 0 && chequearMotivoExit(procesoPCB) < 0 && chequearRecursos(procesoPCB) < 0) // && leQuedaQuantum(procesoDelCPU) < 0 => se evalua luego.
            {
                agregarProcesoAReadyCorrespondiente(procesoPCB);
            }
        }
    }
}

int chequearRecursos(Pcb *proceso) // HEADER
{

    if (procesoDelCPU->motivo == PETICION_RECURSO)
    {
        char **instruccion_separada = string_split(procesoDelCPU->instruccion, " ");

        if (instruccion_separada[0] == "WAIT")
        {
            hacerWAIT(instruccion_separada[1], proceso);
            return 1;
        }
        else
        {
            hacerPOST(instruccion_separada[1]);
            return -1;
        }
    }
}

void hacerWAIT(char *id_recurso, proceso) // HEADER
{
    pthread_t hilo_para_wair_recurso;
    pthread_create(&hilo_para_wair_recurso, NULL, waitHilo, (void *)id_recurso);
    pthread_detach(hilo_para_wair_recurso);
}

void *waitHilo (void *ptr) // HEADER
{
    char *id_recurso = (char *)ptr;

    sem_wait(&((sem_t)dictionary_get(recursos, id_recurso)))

    agregarProcesoAReadyCorrespondiente(proceosPCB_HILO_recursos);
}

void hacerPOST(char *id_recurso) // HEADER
{
    sem_post(&((sem_t)dictionary_get(recursos, id_recurso)));
}

int chequearMotivoIO(Pcb *proceso)
{
    if (procesoDelCPU->motivo != INTERRUPCION_IO)
        return -1;

    if (verificarIOConectada() < 0)
    {
        terminarProceso(proceso);
        return -1;
    }
    procesoDelCPU->pcb->estado = BLOCK;
    enviarProcesoColaIOCorrespondiente(proceso);
    return 1;
}

int verificarIOConectada()
{
    t_list *listaDeIDs = devolverKeys(interfaces_conectadas);
    char *id_io = string_split(procesoDelCPU->instruccion, " ")[1];
    int resultado = -1;

    void verificarIO(char *key, void *value)
    {
        IOguardar_kernel *io_guardada = (IOguardar_kernel *)value;

        if (strcmp(key, id_io) == 0)
        {
            if (io_guardada->interfaz == GENERICA &&
                strcmp(string_split(procesoDelCPU->instruccion, " ")[0], "IO_GEN_SLEEP"))
            {
                resultado = 1;
            }
            else if (io_guardada->interfaz == STDIN &&
                     strcmp(string_split(procesoDelCPU->instruccion, " ")[0], "IO_STDIN_READ"))
            {
                resultado = 1;
            }
            else if (io_guardada->interfaz == STDOUT &&
                     strcmp(string_split(procesoDelCPU->instruccion, " ")[0], "IO_STDOUT_WRITE"))
            {
                resultado = 1;
            }
        }
    }

    buscarNuevasConectadas(interfaces_conectadas, verificarIO);

    return resultado;
}

int chequearMotivoExit(Pcb *proceso)
{
    if (procesoDelCPU->motivo != EXIT_SIGNAL)
        return -1;

    terminarProceso(proceso); // FALTA HACER
    return 1;
}

/*int leQuedaQuantum(MensajeProcesoDelCPU *proceso)
{
    if (proceso->pcb->quantumRestante == 0)
        return -1;
manageIO
    agregarReadyMayorPrioridad(proceso);
    return 1;
}*/

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

        if (algoritmo == VRR)
        {
            procesoDelCPU->pcb->quantumRestante = tiempoRestante;
        }
        else
        {
            procesoDelCPU->pcb->quantumRestante = 0;
        }
    }
    else
    {
        procesoDelCPU->pcb->quantumRestante = 0;
    }

    pthread_join(hiloEscuchaCPUDispatch, NULL);
}

int esperarQuantum(int quantum)
{
    tiempoEjecutando = temporal_create();

    while (1)
    {
        if (*PIDprocesoEjecutando < 0)
        {
            return temporal_gettime(tiempoEjecutando);
        }
        if (temporal_gettime(tiempoEjecutando) >= quantum)
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

    default:
        break;
    }

    enviarMensaje(socket, buffer, KERNEL, MENSAJE);
}

void *escuchaDispatch(void *ptr)
{
    TipoModulo *modulo = get_modulo_msg_recv(&(params->KernelSocketCPUDispatch));
    *PIDprocesoEjecutando = -1; // llego el proceso, ya no esta ejecutando ninguno
    op_code *op_code = get_opcode_msg_recv(&(params->KernelSocketCPUDispatch));

    t_buffer *buffer = buffer_leer_recv(&(params->KernelSocketCPUDispatch));

    procesoDelCPU->motivo = buffer_read_uint32(buffer);
    procesoDelCPU->pcb->pid = buffer_read_uint32(buffer);
    procesoDelCPU->pcb->pc = buffer_read_uint32(buffer);
    obtener_registros_pcbCPU(buffer, procesoDelCPU);

    if (procesoDelCPU->motivo == INTERRUPCION_IO)
    {
        int len = buffer_read_uint32(buffer);
        procesoDelCPU->instruccion = buffer_read_string(buffer, len);
    }

    buffer_destroy(buffer);
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
}

void guardarEnColaGenerico(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIOGenerico);

    queue_push(lista_bloqueados_generico, proceso);

    pthread_mutex_unlock(&mutexIOGenerico);
}

void guardarEnColaSTDIN(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIOSTDIN);

    queue_push(lista_bloqueados_STDIN, proceso);

    pthread_mutex_unlock(&mutexIOSTDIN);
}

void guardarEnColaSTDOUT(structGuardarProcesoEnBloqueado *proceso) // Tiene que guardar la PCB junto a la instruccion
{
    pthread_mutex_lock(&mutexIOSTDOUT);

    queue_push(lista_bloqueados_STDOUT, proceso);

    pthread_mutex_unlock(&mutexIOSTDOUT);
}

void *manageBloqueados(void *ptr) // iniciar hilo en el planificador
{
    (void)ptr;

    pthread_t hiloBloqueadosGenericos;
    pthread_t hiloBloqueadosSTDIN;
    pthread_t hiloBloqueadosSTDOUT;

    pthread_create(&hiloBloqueadosGenericos, NULL, manageIO_Kernel, (void *)GENERICA);
    pthread_create(&hiloBloqueadosSTDIN, NULL, manageIO_Kernel, (void *)STDIN);
    pthread_create(&hiloBloqueadosSTDOUT, NULL, manageIO_Kernel, (void *)STDOUT);

    pthread_detach(hiloBloqueadosGenericos);
    pthread_detach(hiloBloqueadosSTDIN);
    pthread_detach(hiloBloqueadosSTDOUT);
}

void *manageIO_Kernel(void *ptr)
{
    TipoInterfaz tipo_interfaz = (TipoInterfaz)ptr;
    t_list *identificadoresIOConectadas;
    t_list *listasPorCadaID;
    listasPorCadaID = list_create();
    structGuardarProcesoEnBloqueado *proceso;

    t_queue *lista_bloqueados = obtenerColaCorrespondiente(tipo_interfaz);

    while (1)
    {
        obtenerKeys(identificadoresIOConectadas);
        ordenarListaConLasIOsConectadas(tipo_interfaz, listasPorCadaID); // HACE UN 'dictionary_iterator' con 'interfaces_conectadas'

        proceso = (structGuardarProcesoEnBloqueado *)queue_pop(lista_bloqueados);

        if (laIOEstaConectada(identificadoresIOConectadas, proceso) > 0)
        {
            guardarEnSuCola(listasPorCadaID, proceso); // Guardar en su cola, tambien se fija si esta conectada
        }
        else
        {
            terminarProceso(proceso->procesoPCB); // EXISTIRAN DOS FUNCIONES: 'terminarEjecucion' que buscara el proceso en el sistema y eliminara lo necesario
                                                  // y 'terminarProceso' que ira dentro de la otra funcion y hara un EXIT del proceso.
        }

        for (int i = 0; i < list_size(listasPorCadaID); i++)
        {
            structParaHiloFORIO *params = malloc(sizeof(structParaHiloFORIO));
            params->lista = list_get(listasPorCadaID, i);
            params->interfaz = tipo_interfaz;
            pthread_t hiloParaID;
            pthread_create(&hiloParaID, NULL, manageGenericoPorID, (void *)params);
            pthread_join(hiloParaID, NULL);
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
    char *ID_IOProceso = string_split(proceso->instruccion, " ")[1];

    void guardarProcesoYInstruccion(void *value)
    {
        listaBlockPorID *io_conectada_por_id = (listaBlockPorID *)value;
        if (strcmp(io_conectada_por_id->identificador, ID_IOProceso) == 0)
        {
            pthread_mutex_lock(&io_conectada_por_id->mutexCola);
            queue_push(io_conectada_por_id->colaBloqueadoPorID, proceso);
            pthread_mutex_unlock(&io_conectada_por_id->mutexCola);
        }
    }

    list_iterate(listasPorCadaID, guardarProcesoYInstruccion);
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
        pthread_exit(NULL);

    sem_wait(&cola->semEsperarBlock);

    if (queue_is_empty(cola->colaBloqueadoPorID))
        pthread_exit(NULL);

    structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)queue_peek(cola->colaBloqueadoPorID); // TIENE QUE AGARRAR EL PCB JUNTO A LA INSTRUCCION

    enviarMensajeAInterfaz(proceso, cola->socket, params->interfaz);

    int result = esperarConfirmacion(cola->socket);

    quitarProcesoDeLaCola(result, cola); // .pop de la cola, tiene un mutex

    sem_post(&cola->semEsperarBlock);

    pthread_exit(NULL);
}

void enviarMensajeAInterfaz(structGuardarProcesoEnBloqueado *proceso, int *socket, TipoInterfaz tipo) // ENVIAR LO NECESARIO PARA TODAS LAS IO
{
    t_buffer *buffer = buffer_create(strlen(proceso->instruccion) + 1);
    buffer_add_string(buffer, strlen(proceso->instruccion) + 1, proceso->instruccion);
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

void quitarProcesoDeLaCola(int resultadoDeLaIO, listaBlockPorID *cola)
{
    pthread_mutex_lock(&(cola->mutexCola));
    structGuardarProcesoEnBloqueado *proceso = (structGuardarProcesoEnBloqueado *)queue_pop(cola->colaBloqueadoPorID);
    pthread_mutex_unlock(&(cola->mutexCola));

    if (resultadoDeLaIO < 0)
        terminarProceso(proceso->procesoPCB);
    else
        agregarProcesoAReadyCorrespondiente(proceso->procesoPCB);

    free(proceso);
}

int hayProcesosPrioritarios()
{
    int resultado;

    pthread_mutex_lock(&mutexMayorPriordad);

    if (queue_is_empty(cola_de_mayor_prioridad))
        resultado = -1;
    else
        resultado = 1;

    pthread_mutex_unlock(&mutexMayorPriordad);

    return resultado;
}

int enviarProcesoMayorPrioridadCPU()
{
    pthread_mutex_lock(&mutexMayorPriordad);
    Pcb *proceso = (Pcb *)queue_pop(cola_de_mayor_prioridad);
    pthread_mutex_unlock(&mutexMayorPriordad);

    enviarMensajeCPUPCBProceso(proceso); // Mandar solo pcb. En la cola de ready guardar la pcb solamente

    *PIDprocesoEjecutando = proceso->pid;

    int quantum = proceso->quantumRestante;

    free(proceso);

    return quantum;
}

void enviarProcesoReadyCPU()
{
    pthread_mutex_lock(&mutexReady);
    Pcb *proceso = (Pcb *)queue_pop(cola_de_ready);
    pthread_mutex_unlock(&mutexReady);

    enviarMensajeCPUPCBProceso(proceso); // Mandar solo pcb. En la cola de ready guardar la pcb solamente

    *PIDprocesoEjecutando = proceso->pid;

    free(proceso);
}

void enviarMensajeCPUPCBProceso(Pcb *proceso)
{
    t_buffer *buffer = buffer_create(sizeof(Pcb));
    buffer_add_uint32(buffer, proceso->pid);
    buffer_add_uint32(buffer, proceso->pc);
    agregarRegistrosAlBuffer(buffer, proceso);

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
    return *PIDprocesoEjecutando;
}

void agregarProcesoAReadyCorrespondiente(Pcb *proceso)
{
    if (algoritmo == VRR && proceso->quantumRestante > 0)
        agregarProcesoColaMayorPrioridad(proceso);
    else
        agregarProcesoColaReady(proceso);
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

    pthread_mutex_unlock(&mutexMayorPriordad);
}

void agregarProcesoColaReady(Pcb *procesoPCB)
{
    pthread_mutex_lock(&mutexReady);

    queue_push(cola_de_ready, procesoPCB);

    pthread_mutex_unlock(&mutexReady);
}
