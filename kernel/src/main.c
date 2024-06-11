#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <utils/enums/algorimos.h>
#include <pthread.h>

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;
t_dictionary *interfaces_conectadas;
t_queue *cola_de_ready;
t_queue *cola_de_new;
t_queue *cola_de_exit;
t_list *lista_bloqueados_generico;
t_list *lista_bloqueados_STDIN;
t_list *lista_bloqueados_STDOUT;
t_list *lista_bloqueados_DialFS;
Algoritmo algoritmoPlanificacion;

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
    t_queue *cola_de_new;
    t_queue *cola_de_exit;
} colasLargoPlazo;

typedef struct
{
    int KernelSocketCPUDispatch;
    int KernelSocketCPUInterrumpt;
    t_dictionary *interfaces_conectadas;
    colasCortoPlazo *colasCP;
} ParamsPCP;


int main(int argc, char *argv[])
{

    interfaces_conectadas = dictionary_create();
    cola_de_ready = queue_create();
    cola_de_new = queue_create();
    cola_de_exit = queue_create();
    lista_bloqueados_generico = list_create();
    lista_bloqueados_STDIN = list_create();
    lista_bloqueados_STDOUT = list_create();
    lista_bloqueados_DialFS = list_create();
    algoritmoPlanificacion = FIFO;

    colasCortoPlazo *colasCP;
    colasCP->cola_de_ready = cola_de_ready;
    colasCP->lista_bloqueados_DialFS = lista_bloqueados_DialFS;
    colasCP->lista_bloqueados_generico = lista_bloqueados_generico;
    colasCP->lista_bloqueados_STDIN = lista_bloqueados_STDIN;
    colasCP->lista_bloqueados_STDOUT = lista_bloqueados_STDOUT;

    int *KernelSocketCPUDispatchPtr = &KernelSocketCPUDispatch;
    int *KernelSocketCPUInterrumptPtr = &KernelSocketCPUInterrumpt;
    int *KernelSocketMemoriaPtr = &KernelSocketMemoria;

    // Conecta el Kernel con los demas modulos
    conectarModuloKernel(KernelSocketMemoriaPtr, KernelSocketCPUDispatchPtr, KernelSocketCPUInterrumptPtr, interfaces_conectadas);

    ParamsPCP *parametrosPlanificadorCortoPlazo;
    parametrosPlanificadorCortoPlazo->interfaces_conectadas = interfaces_conectadas;
    parametrosPlanificadorCortoPlazo->KernelSocketCPUDispatch = KernelSocketCPUDispatch;
    parametrosPlanificadorCortoPlazo->KernelSocketCPUInterrumpt = KernelSocketCPUInterrumpt;
    parametrosPlanificadorCortoPlazo->colasCP = colas

    // Hilo para el planificador de corto plazo con los datos necesarios introducidos en el struct ParamsPCP
    pthread_t hiloPlanificadorCortoPlazo;
    pthread_create(&hiloPlanificadorCortoPlazo, NULL, planificarCortoPlazo, (void *)parametrosPlanificadorCortoPlazo);

    /*
        El planificador de largo plazo se llamara cuando se quiera crear un proceso y cuando se quiera terminar uno
    */

    /*
        En el PCB el registro PC tiene que ir por separado del resto
    */

    consolaInteractiva();

    pthread_join(hiloPlanificadorCortoPlazo, NULL);
    log_debug(logger_kernel, "Salida del Kernel");
    return 0;
}
