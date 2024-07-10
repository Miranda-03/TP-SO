#include "Conexion/conectar.h"

typedef struct
{
    int socket;
    t_dictionary *interfaces_conectadas;
} parametros_hilo_IO_Kernel;

void conectarModuloKernel(int *KernelSocketMemoria, int *KernelSocketCPUDispatch, int *KernelSocketCPUInterrumpt, t_dictionary *interfaces_conectadas)
{
    t_config *config = config_create(PATH_CONFIG);
    // PUERTO_MEMORIA, IP_MEMORIA
    *KernelSocketMemoria = crearSocket(config_get_string_value(config,"PUERTO_MEMORIA"), config_get_string_value(config,"IP_MEMORIA"), 0);
    handshakeKernelMemoria(KernelSocketMemoria);

    // Conexiones con el módulo CPU 
    *KernelSocketCPUDispatch = crearSocket(config_get_string_value(config,"PUERTO_CPU_DISPATCH"), config_get_string_value(config,"IP_CPU"), 0);
    handshakeKernelCPU(DISPATCH, KernelSocketCPUDispatch);
    sleep(5);
    *KernelSocketCPUInterrumpt = crearSocket(config_get_string_value(config,"PUERTO_CPU_INTERRUPT"), config_get_string_value(config,"IP_CPU"), 0);
    handshakeKernelCPU(INTERRUMPT, KernelSocketCPUInterrumpt);

    // Recibir conexiones de IO
    int KernelsocketEscucha = crearSocket(config_get_string_value(config,"PUERTO_ESCUCHA"), NULL, MAXCONN);
    parametros_hilo_IO_Kernel *params = malloc(sizeof(parametros_hilo_IO_Kernel));
    params->socket = KernelsocketEscucha;
    params->interfaces_conectadas = interfaces_conectadas;
    inicializarMutexDiccionarioIOConectadas();
    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)params);
    pthread_detach(threadClientes);

    config_destroy(config);
}

void *recibirClientes(void *ptr)
{
    parametros_hilo_IO_Kernel *params = (parametros_hilo_IO_Kernel *)ptr; // Castear correctamente el descriptor de socket
    int socketEscucha = params->socket;
    while (1)
    {
        pthread_t thread;

        int *socketBidireccional = malloc(sizeof(int));
        *socketBidireccional = accept(socketEscucha, NULL, NULL);
        if (socketBidireccional < 0)
        {
            perror("accept");
            free(socketBidireccional);
            continue;
        }

        params->socket = *socketBidireccional;
        pthread_create(&thread, NULL, atenderIO, (void *)params); // Pasar el descriptor de socket como un puntero
        pthread_join(thread, NULL);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderIO(void *ptr)
{
    parametros_hilo_IO_Kernel *params = (parametros_hilo_IO_Kernel *)ptr; // Castear correctamente el descriptor de socket
    int socket = params->socket;

    TipoModulo *modulo = get_modulo_msg_recv(&socket);
    op_code *codigoOperacion = get_opcode_msg_recv(&socket);

    if (*codigoOperacion == MENSAJE && *modulo == IO)
    {
        t_buffer *buffer = buffer_leer_recv(&socket);
        TipoInterfaz interfaz = buffer_read_uint32(buffer);
        int sizeIdentificador = buffer_read_uint32(buffer);
        char *identificador = buffer_read_string(buffer, sizeIdentificador);

        guardar_interfaz_conectada(&socket, interfaz, identificador, params->interfaces_conectadas);
        buffer_destroy(buffer);

        enviarPaqueteResult(1, &socket, IO, KERNEL);
    }
    else
    {
        enviarPaqueteResult(-1, &socket, IO, KERNEL);
    }
    free(codigoOperacion);
    free(modulo);
    pthread_exit(NULL);
}

void handshakeKernelCPU(TipoConn conn, int *socket)
{
    t_buffer *buffer = buffer_create(sizeof(TipoConn));
    if (!buffer)
    {
        perror("buffer_create");
        return;
    }
    buffer_add_uint32(buffer, conn);

    enviarMensaje(socket, buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(socket) == 1)
    {
        printf("se conecta con cpu\n");
    }
    else
    {
        printf("error de conexion con cpu\n");
    }
}

void handshakeKernelMemoria(int *socketMemoria)
{
    t_buffer *buffer = buffer_create(0);
    if (!buffer)
    {
        perror("buffer_create");
        return;
    }

    enviarMensaje(socketMemoria, buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(socketMemoria) == 1)
        printf("se conecta con la memoria\n");
    else
        printf("error con la conexion de memoria\n");
}

/*void crearHiloDISPATCH(int *socket, MotivoDesalojo *motivo, int *pid, Registros *registros, instruccionIO *instruccion)
{
    pthread_t hiloDISPATCHKernel;

    parametros_hilo_Kernel *params = malloc(sizeof(parametros_hilo_Kernel));
    params->socket = socket;
    // params->registros = registros;
    params->motivo = motivo;
    params->pid = pid;
    params->instruccion = NULL;

    pthread_create(&hiloDISPATCHKernel,
                   NULL,
                   (void *)manageDISPATCH,
                   params);

    pthread_join(hiloDISPATCHKernel, NULL);
}*/
