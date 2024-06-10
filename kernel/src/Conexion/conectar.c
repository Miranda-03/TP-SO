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
int KernelsocketIO;

void conectarModuloKernel()
{

    KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)(intptr_t)KernelsocketEscucha);

    // Conectar con Memoria
    KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), 0);
    handshakeKernelMemoria(&KernelSocketMemoria);

    // Conexiones con el módulo CPU
    KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(DISPATCH);
    KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), obtenerValorConfig(PATH_CONFIG, "IP_CPU"), NULL);
    handshakeKernelCPU(INTERRUMPT);
}

void *recibirClientes(void *ptr)
{
    int socketEscucha = (intptr_t)ptr; // Castear correctamente el descriptor de socket

    while (1)
    {
        pthread_t thread;

        printf("esperando accept\n");
        int *socketBidireccional = malloc(sizeof(int));
        *socketBidireccional = accept(socketEscucha, NULL, NULL);
        if (socketBidireccional < 0)
        {
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

void *atenderIO(int *socket, TipoConn *conexion)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *codigoOperacion = get_opcode_msg_recv(socket);
    printf("LLEGA A ATENDER IO \n");
    if (*codigoOperacion == HANDSHAKE && *modulo == IO)
    {
        enviarPaqueteResult(1, socket, IO, KERNEL);

        TipoInterfaz *interfaz = buffer_read_uint32(buffer);
        int sizeIdentificador = buffer_read_uint32(buffer);
        char *identificador = buffer_read_string(buffer, sizeIdentificador);
        guardar_interfaz_conectada(socket, *interfaz, identificador, ) // poner el diccionario
    }
    else
    {
        enviarPaqueteResult(-1, socket, IO, KERNEL);
    }
    // free(codigoOperacion);
}

void handshakeKernelCPU(TipoConn conn)
{
    t_buffer *buffer = buffer_create(sizeof(TipoConn));
    if (!buffer)
    {
        perror("buffer_create");
        return;
    }
    buffer_add_uint32(buffer, conn);

    enviarMensaje(&socketSegunConn(conn), buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(&socketSegunConn(conn)) == 1)
    {
        if(conn == DISPATCH){
            crearHiloDISPATCH(&socketSegunConn(conn))
        }
        printf("Handshake KERNEL CPU exitoso\n");
    }
    else
    {
        printf("Handshake KERNEL CPU mal\n");
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
        printf("Handshake KERNEL MEMORIA exitoso \n");
    else
        printf("Handshake KERNEL MEMORIA mal \n");
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

void crearHiloDISPATCH(int *socket, MotivoDesalojo *motivo, int *pid, Registros *registros, instruccionIO *instruccion)
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
}
