#include <Conexion/conectar.h>


void conectarModuloKernel()
{
    int KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);
    
    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)KernelsocketEscucha);
    pthread_join(threadClientes, NULL);

    // Conexiones con el módulo CPU
   /* int KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    int KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);

    // Conexion con el módulo memoria
    int KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL); */
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
                       (void *)atenderIo,
                       socketBidireccional);
        pthread_detach(thread);
    }
}

void *atenderIo(void *ptr)
{
    int socketComunicacion = *((int *)ptr);

    t_resultHandShake *result = malloc(sizeof(t_paquete));

    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(socketComunicacion, &(paquete->modulo), sizeof(TipoModulo), 0);
    recv(socketComunicacion, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socketComunicacion, paquete->buffer->stream, paquete->buffer->size, 0);

    result->moduloRemitente = paquete->modulo;

    result->moduloResponde = MEMORIA;

    switch (paquete->modulo)
    {
    case IO:
        manageIO(socketComunicacion, paquete->buffer, result);
        break;
    
    case CPU:
    default:
        enviarPaqueteResult(result, -1, &socketComunicacion);
        break;
    }

    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
    free(result);
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