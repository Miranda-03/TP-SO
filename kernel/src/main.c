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
    char *charAlgortimo = obtenerValorConfig("kernel.config", "ALGORITMO_PLANIFICACION");
    if (strcmp(charAlgortimo, "RR") == 0)
        algoritmoPlanificacion = RR;
    else if (strcmp(charAlgortimo, "VRR") == 0)
        algoritmoPlanificacion = VRR;
    else
        algoritmoPlanificacion = FIFO;
}

void obtenerRecuros()
{
    char **recursosIDs = string_get_string_as_array(obtenerValorConfig("kernel.config", "RECURSOS"));
    char **recursosCantidad = string_get_string_as_array(obtenerValorConfig("kernel.config", "INSTANCIAS_RECURSOS"));

    int lenIDS = 0;

    while (recursosIDs[lenIDS] != NULL)
    {
        lenIDS++;
    }

    for(int i = 0; i < lenIDS; i++)
    {
        sem_t cant_recurso;
        sem_init(&cant_recurso, 0, atoi(recursosCantidad[i]));
        dictionary_put(recursos_main, recursosIDs[1], &cant_recurso);
    }
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
