#include <Conexion/conectar.h>
int CPUSocketMemoria;

void conectarModuloCPU(int *no, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion)
{
    // Conexion con el módulo memoria
    CPUSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);

    handshakeCPUMemoria();

    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalDispatch = esperarCliente(CPUsocketEscuchaDispatch);
    if (*CPUsocketBidireccionalDispatch != -1)
        recibirConn(CPUsocketBidireccionalDispatch, DISPATCH, procesoCPU, interrupcion);

    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalInterrupt = esperarCliente(CPUsocketEscuchaInterrupt);
    if (*CPUsocketBidireccionalInterrupt != -1)
        recibirConn(CPUsocketBidireccionalInterrupt, INTERRUMPT, procesoCPU, interrupcion);
}

void handshakeCPUMemoria()
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, 2);
    enviarMensaje(CPUSocketMemoria, buffer, CPU, HANDSHAKE);
    //buffer_destroy(buffer); // Asegúrate de destruir el buffer para evitar fugas de memoria
    printf("pasa\n");
    int resultado = resultadoHandShake(CPUSocketMemoria);
    
    if (resultado == 1)
    {
        // Handshake OK
        printf("El handshake de CPU a memoria salió bien\n");
    }
    else
    {
        // Handshake ERROR
        printf("El handshake de CPU a memoria salió mal\n");
        printf("Código de error: %d\n", resultado); // Asume que resultadoHandShake devuelve un código de error
    }
}

void recibirConn(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion)
{
    TipoModulo *modulo = malloc(sizeof(TipoModulo));
    recv(socket, modulo, sizeof(TipoModulo), MSG_WAITALL);

    switch (*modulo)
    {
    case KERNEL:
        manageKernel(socket, conexion, procesoCPU, interrupcion);
        break;

    default:
        break;
    }

    free(modulo);
}

void manageKernel(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion)
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

void crearHiloDISPATCH(int *socket, Contexto_proceso *procesoCPU){
    pthread_t hiloDISPATCH;

    parametros_hilo *params = malloc(sizeof(parametros_hilo));
    params->socket = socket;
    params->interrupcion = NULL;
    params->procesoCPU = procesoCPU;
 
    pthread_create(&hiloDISPATCH,
                       NULL,
                       (void *)manageDISPATCH,
                       params);

    pthread_detach(hiloDISPATCH);        
}

void crearHiloINTERRUPT(int *socket, int *interrupcion){
    pthread_t hiloINTERRUPT;

    parametros_hilo *params = malloc(sizeof(parametros_hilo));
    params->socket = socket;
    params->interrupcion = interrupcion;
    params->procesoCPU = NULL;

    pthread_create(&hiloINTERRUPT,
                       NULL,
                       (void *)manageINTERRUPT,
                       params);

    pthread_detach(hiloINTERRUPT);        
}