#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <utils/structs/structProcesos.h>
#include <utils/enums/motivosDesalojo.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include <commons/collections/queue.h>
#include <utils/enums/codigosOperacion.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/enums/EstadoDeEjecucion.h>
#include <stdio.h>
#include <pthread.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/enums/algorimos.h>
#include <Interfaces/interfaces.h>
#include <semaphore.h>
#include <Planificadores/planificadorLP.h>
#include <commons/log.h>

#define PATH_CONFIG "kernel.config"

typedef struct
{
    t_queue *cola_de_ready;
    t_list *lista_bloqueados_generico;
    t_list *lista_bloqueados_STDIN;
    t_list *lista_bloqueados_STDOUT;
    t_list *lista_bloqueados_DialFS;
} colasCortoPlazo;

typedef struct
{
    sem_t semEsperarBlock;
    pthread_mutex_t mutexCola;
    char *identificador;
    int *conectado;
    int socket;
    t_queue *colaBloqueadoPorID;
} listaBlockPorID;

typedef struct
{
    Pcb *procesoPCB;
    char *instruccion;
} structGuardarProcesoEnBloqueado;

typedef struct
{
    int KernelSocketCPUDispatch;
    int KernelSocketCPUInterrumpt;
    int KernelSocketMemoria;
    Algoritmo algoritmo;
    t_dictionary *interfaces_conectadas;
    t_dictionary *recursos;
} ParamsPCP_kernel;

typedef struct
{
    MotivoDesalojo motivo;
    char *instruccion;
    Pcb *pcb;
} MensajeProcesoDelCPU;

typedef struct
{
    TipoInterfaz interfaz;
    int socket;
} IOguardar_kernel;

typedef struct
{
    listaBlockPorID *lista;
    TipoInterfaz interfaz;
} structParaHiloFORIO;

typedef struct
{
    char *id_recurso;
    Pcb *proceso;
} params_hilo_wait_recurso;

typedef struct
{
    char *id_recurso;
    t_queue *cola_de_bloqueados_por_recurso;
    int *cantidad_recurso;
    int cant_recursos_iniciales;
    pthread_mutex_t mutex_recurso;
    sem_t sem_recursos;
    sem_t procesos_en_espera;
} Recurso;

typedef struct
{
    int pid;
    char *id_recurso;
} Proceso_con_recurso;

/**
 * @fn    planificarCortoPlazo
 * @brief planificador de corto plazo.
 */
void *planificarCortoPlazo(void *ptr);

/**
 * @fn    escuchaDispatch
 * @brief funcion para del hilo para esuchar al modulo CPU en espera de recibir el proceso junto a mas informacion necesaria.
 */
void *escuchaDispatch(void *ptr);

/**
 * @fn    chequearMotivoIO
 * @brief funcion para verificar que la instruccion sea de IO.
 */
int chequearMotivoIO(Pcb *proceso);

/**
 * @fn    chequearMotivoExit
 * @brief funcion para verificar que la instruccion sea EXIT.
 */
int chequearMotivoExit(Pcb *proceso);

/**
 * @fn    esperarProcesoCPU
 * @brief funcion para esperar el proceso de que esta en la CPU, y si el elgoritmo es RR o VRR le manda una interrupcion por fin de quantum.
 */
void esperarProcesoCPU(int quantum);

/**
 * @fn    esperarQuantum
 * @brief mandar interrupcion por fin de quantum.
 */
int esperarQuantum(int quantum);

/**
 * @fn    obtener_registros_pcbCPU
 * @brief obtener los registros generales del proceso que viene del CPU.
 */
void obtener_registros_pcbCPU(t_buffer *buffer, MensajeProcesoDelCPU *proceso);

/**
 * @fn    enviarProcesoColaIOCorrespondiente
 * @brief se fija en que cola de bloqueado tiene que poner el proceso.
 */
void enviarProcesoColaIOCorrespondiente(Pcb *proceso);

/**
 * @fn    guardarEnColaGenerico
 * @brief guarda el proceso en la cola de bloqueado de generico.
 */
void guardarEnColaGenerico(structGuardarProcesoEnBloqueado *proceso);

/**
 * @fn    guardarEnColaSTDIN
 * @brief guarda el proceso en la cola de bloqueado de STDIN.
 */
void guardarEnColaSTDIN(structGuardarProcesoEnBloqueado *proceso);

/**
 * @fn    guardarEnColaSTDOUT
 * @brief guarda el proceso en la cola de bloqueado de STDOUT.
 */
void guardarEnColaSTDOUT(structGuardarProcesoEnBloqueado *proceso);

/**
 * @fn    manageBloqueados
 * @brief crea los hilos para poder manejar los procesos bloqueados.
 */
void *manageBloqueados(void *ptr);

/**
 * @fn    manageIO
 * @brief funcion del hilo para manejar los procesos bloqueados.
 */
void *manageIO_Kernel(void *ptr);

/**
 * @fn    obtenerColaCorrespondiente
 * @brief obtiene la cola de bloqueados correspondiente para el tipo de interfaz.
 */
t_queue *obtenerColaCorrespondiente(TipoInterfaz tipo_interfaz);

/**
 * @fn    obtenerKeys
 * @brief obtiene los identificadores de las interfaces conectadas.
 */
void obtenerKeys(t_list *identificadoresIOConectadas);

/**
 * @fn    ordenarListaConLasIOsConectadas
 * @brief modifica las listas con las interfaces conectadas.
 */
t_list *ordenarListaConLasIOsConectadas(TipoInterfaz tipo, t_list *listasPorCadaID);

/**
 * @fn    PonerIO
 * @brief pone la IO en la lista.
 */
void PonerIO(char *key, void *value, t_list *listasPorCadaID, TipoInterfaz tipo);

/**
 * @fn    MarcarDesconetadas
 * @brief marca las interfaces de la lista como desconectadas.
 */
void MarcarDesconetadas(t_list *listasPorCadaID);

/**
 * @fn    manageGenericoPorID
 * @brief la funcion del hilo para enviar mensaje a la interfaz correspondiente (no importa que diga generico).
 */
void *manageGenericoPorID(void *ptr);

/**
 * @fn    hayProcesosPrioritarios
 * @brief se fija si hay procesos en la cola de Mayor prioridad.
 */
int hayProcesosPrioritarios();

/**
 * @fn    enviarProcesoMayorPrioridadCPU
 * @brief envia a ejecutar el proceso de la cola de mayor prioridad.
 */
int enviarProcesoMayorPrioridadCPU();

/**
 * @fn    enviarProcesoReadyCPU
 * @brief envia a ejecutar el proceso de la cola de Ready.
 */
void enviarProcesoReadyCPU();

/**
 * @fn    hayAlgunoEnCPU
 * @brief se fija si hay algun proceso ejecutando.
 */
int hayAlgunoEnCPU();

/**
 * @fn    agregarProcesoAReadyCorrespondiente
 * @brief agrega el proceso a Ready o a mayor prioridad.
 */
void agregarProcesoAReadyCorrespondiente(Pcb *procesoPCB);

/**
 * @fn    guardarLosRegistros
 * @brief guarda los registros que vienen del proceso del CPU.
 */
void guardarLosRegistros(Pcb *proceso);

/**
 * @fn    agregarProcesoColaMayorPrioridad
 * @brief agrega el proceso a la cola de mayor prioridad.
 */
void agregarProcesoColaMayorPrioridad(Pcb *procesoPCB);

/**
 * @fn    agregarProcesoColaReady
 * @brief agrega el proceso a la cola Ready.
 */
void agregarProcesoColaReady(Pcb *procesoPCB);

/**
 * @fn    quitarProcesoDeLaCola
 * @brief quita el proceso de la cola correspondiente y libera la memoria reservada.
 */
void quitarProcesoDeLaCola(int resultadoDeLaIO, listaBlockPorID *cola, int pid);

/**
 * @fn    agregarRegistrosAlBuffer
 * @brief agrega los registros al buffer.
 */
void agregarRegistrosAlBuffer(t_buffer *buffer, Pcb *proceso);

/**
 * @fn    enviarMensajeCPUPCBProceso
 * @brief envia el proceso a la CPU.
 */
void enviarMensajeCPUPCBProceso(Pcb *proceso);

/**
 * @fn    esperarConfirmacion
 * @brief espera a la confirmacion de finalizacion de la IO.
 */
int esperarConfirmacion(int *socket);

/**
 * @fn    enviarMensajeAInterfaz
 * @brief le envia el mensaje a la IO.
 */
void enviarMensajeAInterfaz(structGuardarProcesoEnBloqueado *proceso, int *socket, TipoInterfaz tipo);

int estaEnLaLista(char *key, t_list *lista);

void guardarEnSuCola(t_list *listasPorCadaID, structGuardarProcesoEnBloqueado *proceso);

int laIOEstaConectada(t_list *conectadas, structGuardarProcesoEnBloqueado *proceso);

void enviarInterrupcion(MotivoDesalojo motivo, int *socket);

int verificarIOConectada(char *instruccion);

int chequearRecursos(Pcb *proceso);

void crearHilosParaWAIT();

void *waitHilo(void *ptr);

void hacerPOST(char *id_recurso, int pid);

int hayProcesosEnCola();

void detenerPlanificador();

void reanudarPlanificador();

/**
 * @fn    recurso_wait
 * @brief funcion para hacer el wait del recurso.
 */
void recurso_wait(char *id_recurso, int *cant_recurso, int pid_solicitante, t_queue *cola_de_bloqueados_recursos, pthread_mutex_t *mutex, sem_t *instancias);

/**
 * @fn    poner_en_lista_de_recursos_adquiridos
 * @brief poner que recurso tiene cada proceso.
 */
void poner_en_lista_de_recursos_adquiridos(int pid, char *id_recurso);

/**
 * @fn    agregar_recurso_al_diccionario
 * @brief suma 1 a la cantidad de undidades del recurso.
 */
void agregar_recurso_al_diccionario(char *id_recurso);

/**
 * @fn    liberar_recursos
 * @brief libera los recursos que tenga el proceso.
 */
void liberar_recursos(Pcb *proceso);

int encontrar_y_terminar_proceso(int pid);

int buscar_en_readys(int pid);

int buscar_en_bloqueados_y_terminar(int pid);

int buscar_bloqueados_porIDio(int pid);

int buscar_colas_recursos_terminar(int pid);

void buscar_en_espera(t_queue *cola, int pid, int *resultado, pthread_mutex_t *mutex);

void interrumpir_ejecucion();

void guardar_en_cola_correspondiente_recurso(Pcb *proceso, char **instruccion_separada);

void log_ingreso_a_ready(char *cola, Pcb *proceso_nuevo);

char *obtener_array_de_pids(char *cola, Pcb *proceso_nuevo);

char *enum_to_string_EXIT(MotivoDesalojo motivo);

char *enum_to_string(EstadoProceso estado);

void mensaje_desalojo(); 

void mensaje_cambio_de_estado(char *estado_anterior, char *estado_siguiente, int pid);

sem_t *obtenerSemaforoCorrespondiente(TipoInterfaz interfaz);

void guardarEnColaDIALFS(structGuardarProcesoEnBloqueado *proceso);

#endif