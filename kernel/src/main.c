#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <utils/enums/algorimos.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>
#include <Planificadores/planificadorLP.h>
#include <Procesos/consola.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <commons/config.h>

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;
t_dictionary *interfaces_conectadas_main;
t_dictionary *recursos_main;

Algoritmo algoritmoPlanificacion;
typedef struct
{
    t_queue *cola_de_new;
    t_queue *cola_de_exit;
} colasLargoPlazo;

void obtenerAlgoritmoDeConfig()
{
    t_config *config = config_create("kernel.config");
    char *charAlgortimo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    if (strcmp(charAlgortimo, "RR") == 0)
        algoritmoPlanificacion = RR;
    else if (strcmp(charAlgortimo, "VRR") == 0)
        algoritmoPlanificacion = VRR;
    else
        algoritmoPlanificacion = FIFO;

    config_destroy(config);
}

void obtenerRecuros()
{
    t_config *config = config_create("kernel.config");
    char **recursosIDs = config_get_array_value(config, "RECURSOS");
    char **recursosCantidad = config_get_array_value(config, "INSTANCIAS_RECURSOS");

    int lenIDS = 0;

    while (recursosIDs[lenIDS] != NULL)
    {
        lenIDS++;
    }

    for (int i = 0; i < lenIDS; i++)
    {
        sleep(1);
        int cant = atoi(recursosCantidad[i]);
        Recurso *recurso = malloc(sizeof(Recurso));
        recurso->cantidad_recurso = malloc(4);
        recurso->cola_de_bloqueados_por_recurso = queue_create();
        pthread_mutex_init(&recurso->mutex_recurso, NULL);
        *(recurso->cantidad_recurso) = cant;
        recurso->id_recurso = strdup(recursosIDs[i]);
        recurso->cant_recursos_iniciales = cant;
        sem_init(&(recurso->sem_recursos), 0, cant);
        sem_init(&(recurso->procesos_en_espera), 0, 0);
        dictionary_put(recursos_main, recursosIDs[i], recurso);
    }

    string_array_destroy(recursosIDs);
    string_array_destroy(recursosCantidad);
    config_destroy(config);
}

int main(int argc, char *argv[])
{

    interfaces_conectadas_main = dictionary_create();

    recursos_main = dictionary_create();

    obtenerAlgoritmoDeConfig();

    obtenerRecuros();

    int *KernelSocketCPUDispatchPtr = &KernelSocketCPUDispatch;
    int *KernelSocketCPUInterrumptPtr = &KernelSocketCPUInterrumpt;
    int *KernelSocketMemoriaPtr = &KernelSocketMemoria;

    // Conecta el Kernel con los demas modulos
    conectarModuloKernel(KernelSocketMemoriaPtr, KernelSocketCPUDispatchPtr, KernelSocketCPUInterrumptPtr, interfaces_conectadas_main);

    ParamsPCP_kernel *parametrosPlanificadorCortoPlazo = malloc(sizeof(ParamsPCP_kernel));
    parametrosPlanificadorCortoPlazo->interfaces_conectadas = interfaces_conectadas_main;
    parametrosPlanificadorCortoPlazo->KernelSocketCPUDispatch = KernelSocketCPUDispatch;
    parametrosPlanificadorCortoPlazo->KernelSocketCPUInterrumpt = KernelSocketCPUInterrumpt;
    parametrosPlanificadorCortoPlazo->KernelSocketMemoria = KernelSocketMemoria;
    parametrosPlanificadorCortoPlazo->algoritmo = algoritmoPlanificacion;
    parametrosPlanificadorCortoPlazo->recursos = recursos_main;

    // Hilo para el planificador de corto plazo con los datos necesarios introducidos en el struct ParamsPCP
    pthread_t hiloPlanificadorCortoPlazo;
    pthread_create(&hiloPlanificadorCortoPlazo, NULL, planificarCortoPlazo, (void *)parametrosPlanificadorCortoPlazo);
    pthread_detach(hiloPlanificadorCortoPlazo);

    // Se inicializa el planificador a largo plazo.
    inicarPlanificadorLargoPLazo(KernelSocketMemoria);

    consolaInteractiva();

    // log_debug(logger_kernel, "Salida del Kernel");
    return 0;
}
