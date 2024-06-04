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
    
    KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)(intptr_t)KernelsocketEscucha);

    KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int *KenerlsocketBidireccionalDispatch = esperarCliente(KernelSocketCPUDispatch);
    if (*KenerlsocketBidireccionalDispatch != -1)
        //recibirConexion(*KenerlsocketBidireccionalDispatch, DISPATCH, procesoCPU, interrupcion);

    KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int *KernelsocketBidireccionalInterrupt = esperarCliente(KernelSocketCPUInterrumpt);
    if (*KernelsocketBidireccionalInterrupt != -1)
        //recibirConexion(*CPUsocketBidireccionalInterrupt, INTERRUMPT, procesoCPU, interrupcion);
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

    int socket = socketSegunConn(conn);
    printf ("socket %i\n", socket);

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

void recibirConexion(int *socket, TipoConn conexion, )
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);

    switch (*modulo)
    {
    case CPU:
        manageCPU(socket, conexion);
        break;
    case IO:
        manageIO(socket,);
    default:
        break;
    }

    free(modulo);
}

void manageCPU(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion)
{
    op_code *codigoOperacion = get_opcode_msg_recv(socket);
    printf("LLEGA AL MANAGE KERNEL DEL CPU \n");
    if (*codigoOperacion == HANDSHAKE)
    {
        enviarPaqueteResult(1, socket, CPU, KERNEL);
    }
    else
    {
        if(conexion == DISPATCH){
            crearHiloDISPATCH(socket, procesoCPU);
        }
        else{
            crearHiloINTERRUPT(socket, interrupcion);
        }
    }

    free(codigoOperacion);
}

void crearHiloDISPATCH(int *socket, MotivoDesalojo* motivo, int* pid, Registros* registros, char* instruccion){
    pthread_t hiloDISPATCHKernel;

    parametros_hilo_Kernel *params = malloc(sizeof(parametros_hilo));
    params->socket = socket;
    params->registros = registros;
    params->MotivoDesalojo = motivo;
    params->pid=pid;
    params->instruccion=NULL;
 
    pthread_create(&hiloDISPATCHKernel,
                       NULL,
                       (void *)manageDISPATCH,
                       params);

    pthread_join(hiloDISPATCHKernel,NULL);        
}

void crearHiloINTERRUPT(int *socket, MotivoDesalojo* motivo,int* pid,Registros* registros,char* instruccion){
    pthread_t hiloINTERRUPTKernel;

    parametros_hilo_Kernel *params = malloc(sizeof(parametros_hilo_Kernel));
    params->socket = socket;
    params->registros = registros;
    params->MotivoDesalojo = motivo;
    params->pid = pid;
    params->instruccion = instruccion;
    
    pthread_create(&hiloINTERRUPTKernel,
                       NULL,
                       (void *)manageINTERRUPT,
                       params);

    pthread_join(hiloINTERRUPTKernel,NULL);        
}

