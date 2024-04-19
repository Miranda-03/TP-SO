#include <Conexion/conectar.h>

void conectarModuloMemoria()
{
    pthread_t threadModulos;

    int MemoriasocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

    pthread_create(&threadModulos, NULL, recibirModulo, (void *)MemoriasocketEscucha);
    pthread_join(threadModulos, NULL);
}

void *recibirModulo(void *ptr)
{
    int MemoriasocketEscucha = (int *)ptr; // Castear correctamente el descriptor de socket                             // Liberar el espacio de memoria reservado en conectarModuloMemoria

    while (1)
    {
        pthread_t thread;
        int *socketBidireccional = malloc(sizeof(int));
        printf("esperando accept\n");
        *socketBidireccional = accept(MemoriasocketEscucha, NULL, NULL);
        printf("aceptado\n");
        pthread_create(&thread,
                       NULL,
                       (void *)atenderModulo,
                       socketBidireccional); // Pasar el descriptor de socket como un puntero
        pthread_detach(thread);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderModulo(void *ptr)
{
    int socketComunicacion = *((int *)ptr);

    t_resultHandShake *result = malloc(sizeof(t_paquete));

    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(socketComunicacion, &(paquete->modulo), sizeof(TipoModulo), 0);

    result->moduloRemitente = paquete->modulo;
    result->moduloResponde = MEMORIA;

    switch (paquete->modulo)
    {
    case IO:
        recv(socketComunicacion, &(paquete->buffer->size), sizeof(uint32_t), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketComunicacion, paquete->buffer->stream, paquete->buffer->size, 0);
        manageIO(socketComunicacion, paquete->buffer, result);
        break;

    case CPU:
        manageCPU(socketComunicacion, result);
        break;
    
    case KERNEL:
        manageKernel(socketComunicacion, result);
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

void manageCPU(int *socket, t_resultHandShake *result)
{
    enviarPaqueteResult(result, 1, socket);
}

void manageIO(int *socket, t_buffer *buffer, t_resultHandShake *result)
{

    void *stream = buffer->stream;

    TipoInterfaz tipo;
    memcpy(&tipo, stream, 4);

    switch (tipo)
    {
    case STDIN:
        enviarPaqueteResult(result, 1, socket);
        break;

    case STDOUT:
        enviarPaqueteResult(result, 1, socket);
        break;

    case DIALFS:
        enviarPaqueteResult(result, 1, socket);
        break;

    default:
        enviarPaqueteResult(result, -1, socket);
        break;
    }
}

void manageKernel(int *socket, t_resultHandShake *result)
{
    enviarPaqueteResult(result, 1, socket);
}