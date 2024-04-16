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
        *socketBidireccional = accept(MemoriasocketEscucha, NULL, NULL);
      
        pthread_create(&thread,
                       NULL,
                       (void *)atenderModulo,
                       socketBidireccional); // Pasar el descriptor de socket como un puntero
        pthread_detach(thread);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderModulo(void *socketComunicacion)
{
    printf("hace cosas con el modulo MEMORIA\n");

    t_resultHandShake *result = malloc(sizeof(t_paquete));

    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(socketComunicacion, &(paquete->modulo),  sizeof(TipoModulo), MSG_WAITALL);
    recv(socketComunicacion, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socketComunicacion, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

    printf("recibe algo MEMORIA\n");   

    result->moduloRemitente = paquete->modulo;
    printf("Valor del módulo MEMORIA: %d\n", (int)paquete->modulo);

    result->moduloResponde = MEMORIA;

    switch (paquete->modulo)
    {
    case IO:
        manageIO(socketComunicacion, paquete->buffer, result);
        break;

    default:
        enviarPaqueteResult(result, -1, socketComunicacion);
        break;
    }

     printf("llega al final MEMORIA\n");   

    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
    free(result);
}