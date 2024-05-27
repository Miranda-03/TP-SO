#include <Conexion/conectar.h>

typedef struct
{
    TipoModulo modulo;
    TipoConn conn;
} HandshakeMessageKernel;

extern t_dictionary *interfaces_conectadas;
extern t_log *logger_kernel;

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;

void conectarModuloKernel()
{
    int KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)(intptr_t)KernelsocketEscucha);

    // Conexiones con el módulo CPU
    KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(DISPATCH);
    KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(INTERRUMPT);

    // Conexion con el módulo memoria
    KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
    handshakeKernelMemoria();
    pthread_join(threadClientes, NULL);
}

void *recibirClientes(void *ptr)
{
    int MemoriasocketEscucha = (intptr_t)ptr; // Castear correctamente el descriptor de socket

    while (1)
    {
        pthread_t thread;
        int *socketBidireccional = malloc(sizeof(int));
        if (!socketBidireccional) {
            perror("malloc");
            return NULL;
        }

        printf("esperando accept\n");
        *socketBidireccional = accept(MemoriasocketEscucha, NULL, NULL);
        if (*socketBidireccional < 0) {
            perror("accept");
            free(socketBidireccional);
            continue;
        }
        printf("aceptado\n");
        pthread_create(&thread, NULL, atenderIO, socketBidireccional); // Pasar el descriptor de socket como un puntero
        pthread_detach(thread);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderIO(void *ptr)
{
    int socketComunicacion = *((int *)ptr);
    free(ptr);

    TipoModulo *moduloEntrante = malloc(sizeof(TipoModulo));
    if (!moduloEntrante) {
        perror("malloc");
        return NULL;
    }

    if (recv(socketComunicacion, moduloEntrante, sizeof(TipoModulo), 0) <= 0) {
        perror("recv");
        free(moduloEntrante);
        return NULL;
    }

    switch (*moduloEntrante)
    {
    case IO:
        manageIO(&socketComunicacion);
        break;
    default:
        // enviarPaqueteResult(result, -1, socketComunicacion);
        break;
    }

    free(moduloEntrante);
    return NULL;
}

void handshakeKernelCPU(TipoConn conn)
{
    t_buffer *buffer = buffer_create(sizeof(TipoConn));
    if (!buffer) {
        perror("buffer_create");
        return;
    }
    buffer_add(buffer, &conn, sizeof(TipoConn));

    enviarMensaje(socketSegunConn(conn), buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(socketSegunConn(conn)) == 1)
    {
        printf("Handshake KERNEL CPU exitoso\n");
    }
    else
    {
        printf("Handshake KERNEL CPU mal\n");
    }

    buffer_destroy(buffer);
}

void manageIO(int *socket)
{
    op_code *opCode = get_opcode_msg_recv(socket);
    if (!opCode) {
        perror("get_opcode_msg_recv");
        return;
    }
    void *stream = buffer_leer_stream_recv(socket);
    if (!stream) {
        perror("buffer_leer_stream_recv");
        free(opCode);
        return;
    }

    TipoInterfaz tipo;
    memcpy(&tipo, stream, sizeof(TipoInterfaz));
    char *identificador = obtener_identificador(stream);
    free(stream);

    if (*opCode == HANDSHAKE)
    {
        guardar_interfaz_conectada(socket, tipo, identificador, interfaces_conectadas);
        enviarPaqueteResult(1, socket, KERNEL, IO);
    }
    else
    {
       enviarPaqueteResult(-1, socket, KERNEL, IO);
    }

    free(identificador);
    free(opCode);
}

void handshakeKernelMemoria()
{
    t_buffer *buffer = buffer_create(0);
    if (!buffer) {
        perror("buffer_create");
        return;
    }
    enviarMensaje(KernelSocketMemoria, buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(KernelSocketMemoria) == 1)
        printf("Handshake KERNEL MEMORIA exitoso \n");
    else
        printf("Handshake KERNEL MEMORIA mal \n");

    buffer_destroy(buffer);
}

char *obtener_identificador(void *stream)
{
    int size_identificador;
    memcpy(&size_identificador, (char *)stream + sizeof(TipoInterfaz), sizeof(int));

    char *identificador = malloc(size_identificador + 1);
    if (!identificador) {
        perror("malloc");
        return NULL;
    }
    memcpy(identificador, (char *)stream + sizeof(TipoInterfaz) + sizeof(int), size_identificador);
    identificador[size_identificador] = '\0';

    return identificador;
}

int socketSegunConn(TipoConn conn)
{
    switch (conn)
    {
    case DISPATCH:
        return KernelSocketCPUDispatch;
    case INTERRUMPT:
        return KernelSocketCPUInterrumpt;
    default:
        return -1;
    }
}
