#include <Conexion/conectar.h>

typedef struct
{
    TipoModulo modulo;
    TipoConn conn;
} HandshakeMessageKernel;

int KernelSocketCPUDispatch;
int KernelSocketCPUInterrumpt;
int KernelSocketMemoria;

t_dictionary *interfaces_conectadas = dictionary_create();

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
    KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
    handshakeKernelMemoria();
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

    TipoModulo *moduloEntrante = malloc(sizeof(TipoModulo));
    recv(socketComunicacion, moduloEntrante, sizeof(TipoModulo), 0);

    switch (*moduloEntrante)
    {
    case IO:
        manageIO(socketComunicacion);
        break;
    default:
        // enviarPaqueteResult(result, -1, socketComunicacion);
        break;
    }

    free(moduloEntrante);
}

void handshakeKernelCPU(TipoConn conn)
{
    t_buffer *buffer = buffer_create(sizeof(TipoConn));
    // buffer_add_uint32(buffer, conn);
    buffer_add(buffer, &conn, 4);
    memcpy(buffer->stream, &conn, sizeof(TipoConn));
    enviarMensaje(socketSegunConn(conn), buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(socketSegunConn(conn)) == 1)
    {
        printf("Handshake KERNEL CPU exitoso\n");
    }
    else
    {
        printf("Handshake KERNEL CPU mal\n");
    }
}



void manageIO(int *socket)
{

    op_code *opCode = get_opcode_msg_recv(socket);
    t_buffer *buffer = buffer_leer_recv(socket);
    TipoInterfaz tipo;
    memcpy(&tipo, buffer->stream, 4);
    char * identificador = obtener_identificador(buffer);
    buffer_destroy(buffer);

    if (*opCode == HANDSHAKE)
    {
        guardar_interfaz_conectada(socket, tipo, identificador, interfaces_conectadas);
        enviarPaqueteResult(1, socket, KERNEL, IO);
    }
    else
    {
        /*
        t_buffer *buffer = buffer_leer_recv(socket);
        TipoInterfaz tipo;
        memcpy(&tipo, buffer->stream, 4);

        switch (tipo) // eliminar el buffer
        {
        case STDIN:
            // funcion para STDIN
            break;

        case STDOUT:
            // funcion para STDOUT
            break;

        case DIALFS:
            // funcion para DIALFS
            break;

        case GENERICA:
            // funcion para GENERICA
            break;
        default:
            // enviar mensaje de error
            break;
        }*/
    }

    free(opCode);
}

void handshakeKernelMemoria()
{
    t_buffer *buffer = buffer_create(0);
    enviarMensaje(KernelSocketMemoria, buffer, KERNEL, HANDSHAKE);
    // buffer_destroy(buffer);

    if (resultadoHandShake(KernelSocketMemoria) == 1)
        printf("Handshake KERNEL MEMORIA exitoso \n");
    else
        printf("Handshake KERNEL MEMORIA mal \n");
}

char *obtener_identificador(t_buffer *buffer){
    int size_identificador;
    char *identificador;
    memcpy(&size_identificador, buffer->stream + 4, sizeof(uint32_t));
    memcpy(identificador, buffer->stream + 4 + sizeof(uint32_t), size_identificador);
    
    return identificador;
}

void guardar_interfaz_conectada(int *socket, TipoInterfaz interfaz, char* identificador){
    
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