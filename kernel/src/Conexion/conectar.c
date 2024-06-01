#include "Conexion/conectar.h"

typedef struct
{
    TipoModulo modulo;
    TipoConn conn;
} HandshakeMessageKernel;

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;
int KernelsocketEscucha;

void conectarModuloKernel()
{
    interfaces_conectadas = dictionary_create();

    KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

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

        printf("esperando accept\n");
        int *socketBidireccional = malloc(sizeof(int));
        *socketBidireccional = accept(MemoriasocketEscucha, NULL, NULL);
        if (socketBidireccional < 0) {
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

    TipoModulo *moduloEntrante = get_modulo_msg_recv(socketComunicacion);

    switch (*moduloEntrante)
    {
    case IO:
        manageIO(socketComunicacion);
        break;
    default:
        enviarPaqueteResult(-1, socketComunicacion, KERNEL, *moduloEntrante);
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
    buffer_add_uint32(buffer, conn);

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
//impresora
    t_buffer *buffer = buffer_leer_recv(socket);

    TipoInterfaz tipo = buffer_read_uint32(buffer);
    int sizeIdentificador = buffer_read_uint32(buffer);
    char *identificador = buffer_read_string(buffer, sizeIdentificador);
    buffer_destroy(buffer);
    if (*opCode == HANDSHAKE)
    {
        guardar_interfaz_conectada(socket, tipo, identificador, interfaces_conectadas);
        enviarPaqueteResult(1, socket, KERNEL, IO);
    }
    else
    {
       enviarPaqueteResult(-1, socket, KERNEL, IO);
    }
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
