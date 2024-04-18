#include <Conexion/conectar.h>

typedef struct
{
    TipoModulo modulo;
    TipoConn conn;
} HandshakeMessageKernel;

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;

void conectarModuloKernel()
{
    int KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)KernelsocketEscucha);
    // Conexiones con el módulo CPU
    KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(DISPATCH);
    KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(INTERRUMPT);

    // Conexion con el módulo memoria
    int KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
    pthread_join(threadClientes, NULL);
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

void handshakeKernelCPU(TipoConn conn)
{
    HandshakeMessageKernel handshakemsg = {KERNEL, conn};

    void *stream = malloc(sizeof(HandshakeMessageKernel));
    int offset = 0;
    memcpy(stream, &(handshakemsg.modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(stream + offset, &(handshakemsg.conn), sizeof(TipoConn));

    send(socketSegunConn(conn), stream, sizeof(HandshakeMessageKernel), 0);
    int resultado = resultadoHandShake(socketSegunConn(conn));

    free(stream);

    if (resultado == 1)
    {
        printf("Handshake KERNEL CPU exitoso\n");
    }
    else
    {
        printf("Handshake KERNEL CPU mal\n");
    }
}

int socketSegunConn(TipoConn conn)
{
    switch (conn)
    {
    case DISPATCH:
        return KernelSocketCPUDispatch;
        break;

    case INTERRUMPT:
        return KernelSocketCPUInterrumpt;
        break;
    default:
        return -1;
        break;
    }
}