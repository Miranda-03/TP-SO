#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <pthread.h>

void conectarModuloKernel()
{
    int KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);
    
    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)KernelsocketEscucha);
    pthread_join(threadClientes, NULL);

    // Conexiones con el módulo CPU
    int KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    int KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);

    // Conexion con el módulo memoria
    int KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
}

void *recibirClientes(void *ptr)
{
    int KernelsocketEscucha = (int *)ptr;
    while (1)
    {
        pthread_t thread;
        int *socketBidireccional = malloc(sizeof(int));
        if ((*socketBidireccional = accept(KernelsocketEscucha, NULL, NULL)) == -1)
        {
            perror("Error al aceptar la conexión");
        }
        pthread_create(&thread,
                       NULL,
                       (void *)atenderIO,
                       socketBidireccional);
        pthread_detach(thread);
    }
}

void *atenderIO(void *socketComunicacion)
{
    printf("hace cosas con el dispositivo i/o\n");
}