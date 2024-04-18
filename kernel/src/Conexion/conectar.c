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
    /*KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(DISPATCH);
    KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(INTERRUMPT); 

    //Conexion con el módulo memoria
    int KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL); */
    pthread_join(threadClientes, NULL); 
}

void *recibirClientes(void *ptr)
{
    int MemoriasocketEscucha = (int *)ptr; // Castear correctamente el descriptor de socket    

    while (1)
    {
        pthread_t thread;
        int *socketBidireccional = malloc(sizeof(int));
        printf("esperando accept\n");
        *socketBidireccional = accept(MemoriasocketEscucha, NULL, NULL);
        printf("aceptado\n");
        pthread_create(&thread,
                       NULL,
                       (void *)atenderIO,
                       socketBidireccional); // Pasar el descriptor de socket como un puntero
        pthread_detach(thread);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderIO(void *ptr)
{
    int socketComunicacion = *((int *)ptr);

    t_resultHandShake *result = malloc(sizeof(t_paquete));

    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(socketComunicacion, &(paquete->modulo), sizeof(TipoModulo), 0);

    result->moduloRemitente = paquete->modulo;
    result->moduloResponde = KERNEL;

    switch (paquete->modulo)
    {
    case IO:
        recv(socketComunicacion, &(paquete->buffer->size), sizeof(uint32_t), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketComunicacion, paquete->buffer->stream, paquete->buffer->size, 0);
        manageIO(socketComunicacion, paquete->buffer, result);
        break;
    default:
        enviarPaqueteResult(result, -1, socketComunicacion);
        break;
    }

    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
    free(result);
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

void manageIO(int *socket, t_buffer *buffer, t_resultHandShake *result)
{

    void *stream = buffer->stream;

    TipoInterfaz tipo;
    memcpy(&tipo, stream, 4);

    switch (tipo)
    {
    case STDIN:
        enviarPaqueteResult(result, 0, socket);
        break;

    case STDOUT:
        enviarPaqueteResult(result, 0, socket);
        break;

    case DIALFS:
        enviarPaqueteResult(result, 0, socket);
        break;

    default:
        enviarPaqueteResult(result, -1, socket);
        break;
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