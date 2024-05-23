#include <Conexion/conectar.h>


void conectarModuloCPU(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Proceso *procesoCPU)
{
    // Conexion con el módulo memoria
    *CPUSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);

    handshakeCPUMemoria(CPUSocketMemoria);

    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalDispatch = esperarCliente(CPUsocketEscuchaDispatch);
    if (*CPUsocketBidireccionalDispatch != -1)
        recibirConn(CPUsocketBidireccionalDispatch, DISPATCH, procesoCPU);

    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalInterrupt = esperarCliente(CPUsocketEscuchaInterrupt);
    if (*CPUsocketBidireccionalInterrupt != -1)
        recibirConn(CPUsocketBidireccionalInterrupt, INTERRUMPT, procesoCPU);
}

void handshakeCPUMemoria(int *CPUSocketMemoria)
{
    t_buffer *buffer = buffer_create(0);
    enviarMensaje(CPUSocketMemoria, buffer, CPU, HANDSHAKE);
    //buffer_destroy(buffer);

    if (resultadoHandShake(CPUSocketMemoria) == 1)
    {
        // Handshake OK
        printf("El handshake de CPU a memoria salio bien\n");
    }
    else
    {
        printf("El handshake de CPU a memoria salio mal\n");
        // Handshake ERROR
    }
}

void recibirConn(int *socket, TipoConn conexion, Proceso *procesoCPU)
{
    TipoModulo *modulo = malloc(sizeof(TipoModulo));
    recv(socket, modulo, sizeof(TipoModulo), MSG_WAITALL);

    switch (*modulo)
    {
    case KERNEL:
        manageKernel(socket, conexion, procesoCPU);
        break;

    default:
        break;
    }

    free(modulo);
}

void manageKernel(int *socket, TipoConn conexion, Proceso *procesoCPU)
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
            crearHiloDISPATCH(socket);
        }
        else{
            crearHiloINTERRUPT(socket);
        }
    }

    free(codigoOperacion);
}

void crearHiloDISPATCH(int *socket, Proceso *procesoCPU){
    pthread_t hiloDISPATCH;

    paremetros_hilo params = {socket, procesoCPU};
 
    pthread_create(&hiloDISPATCH,
                       NULL,
                       (void *)manageDISPATCH,
                       params);

    pthread_detach(hiloDISPATCH);        
}

void crearHiloINTERRUPT(int *socket, Proceso *procesoCPU){
    pthread_t hiloINTERRUPT;

    paremetros_hilo params = {socket, procesoCPU};
    
    pthread_create(&hiloINTERRUPT,
                       NULL,
                       (void *)manageINTERRUPT,
                       params);

    pthread_detach(hiloINTERRUPT);        
}