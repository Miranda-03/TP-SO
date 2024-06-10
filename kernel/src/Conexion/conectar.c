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
    
    /*KernelsocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, MAXCONN);

    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)(intptr_t)KernelsocketEscucha);*/

    KernelSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
    handshakeKernelMemoria(&KernelSocketMemoria);
    

    KernelSocketCPUDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int *KernelsocketBidireccionalDispatch = esperarCliente(&KernelSocketCPUDispatch);
    if (*KernelsocketBidireccionalDispatch != -1)
        recibirConexion(*KernelsocketBidireccionalDispatch, DISPATCH);

    KernelSocketCPUInterrumpt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_CPU_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int *KernelsocketBidireccionalInterrupt = esperarCliente(&KernelSocketCPUInterrumpt);
    if (*KernelsocketBidireccionalInterrupt != -1)
        recibirConexion(*KernelsocketBidireccionalInterrupt, INTERRUMPT);
    
    KernelsocketIO = crearSocket(PATH_CONFIG,"PUERTO_IO",MAXCONN);
    int *KernelsocketBidireccionalIO = esperarCliente(KernelsocketIO);
    if(*KernelsocketBidireccionalIO!= -1)
        recibirConexion(*KernelsocketBidireccionalIO,IO);

    
}

/*void *recibirClientes(void *ptr)
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
}*/

/*void *atenderIO(int* socket,TipoConn* conexion)
{
    op_code *codigoOperacion = get_opcode_msg_recv(socket);
    printf("LLEGA A ATENDER IO \n");
    if(*codigoOperacion==HANDSHAKE)
    {
        enviarPaqueteResult(1, socket, IO, KERNEL);
    }
    else{
        manageIO(socket,TipoConn* conexion);
    }
    free(codigoOperacion);
}*/

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


void handshakeKernelMemoria(int *socketMemoria)
{
    t_buffer *buffer = buffer_create(0);
    if (!buffer) {
        perror("buffer_create");
        return;
    }
    
    enviarMensaje(socketMemoria, buffer, KERNEL, HANDSHAKE);

    if (resultadoHandShake(socketMemoria) == 1)
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

void recibirConexion(int *socket, TipoConn conexion)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);

    switch (*modulo)
    {
    case CPU:
        manageCPU(socket, conexion);
        break;
    case IO:
        crearHiloIO(socket);
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

void crearHiloDISPATCH(int *socket, MotivoDesalojo* motivo, int* pid, Registros* registros, instruccionIO* instruccion){
    pthread_t hiloDISPATCHKernel;

    parametros_hilo_Kernel *params = malloc(sizeof(parametros_hilo_Kernel));
    params->socket = socket;
    //params->registros = registros;
    params->motivo = motivo;
    params->pid=pid;
    params->instruccion=NULL;
 
    pthread_create(&hiloDISPATCHKernel,
                       NULL,
                       (void *)manageDISPATCH,
                       params);

    pthread_join(hiloDISPATCHKernel,NULL);        
}

void crearHiloINTERRUPT(int *socket, MotivoDesalojo* motivo, int* pid, Registros* registros, instruccionIO* instruccion){
    pthread_t hiloINTERRUPTKernel;

    parametros_hilo_Kernel *params = malloc(sizeof(parametros_hilo_Kernel));
    params->socket = socket;
    //params->registros = registros;
    params->motivo = motivo;
    params->pid = pid;
    params->instruccion = instruccion;
    
    pthread_create(&hiloINTERRUPTKernel,
                       NULL,
                       (void *)manageINTERRUPT,
                       params);

    pthread_join(hiloINTERRUPTKernel,NULL);        
}

void crearHiloIO(int* socket,TipoInterfaz* tipo,char* identificador)
{
    /*pthread_t hilo_IO_Kernel;
        parametros_hilo_IO *params = malloc(sizeof(parametros_hilo_IO));
        params->socket=socket;
        params->interfaz=tipo;
        params->identificador=identificador;
         pthread_create(&hilo_IO_Kernel,
                       NULL,
                       (void *)manageIO,
                       params);

    pthread_join(hilo_IO_Kernel,NULL); */
}
