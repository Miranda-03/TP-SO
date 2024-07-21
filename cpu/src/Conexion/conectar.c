#include <Conexion/conectar.h>

t_log *loger;

void conectarModuloCPU(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion)
{
    loger = log_create("logs/cpu.log", "cpu_conn", 1, LOG_LEVEL_INFO);

    //Obtener direccion IP del modulo memoria
    t_config *cpu_config = config_create("cpu.config");
    solicitar_ip("255.255.255.255", config_get_string_value(cpu_config, "PUERTO_MEMORIA"), cpu_config, "IP_MEMORIA", loger);
    config_destroy(cpu_config);

    // Conexion con el módulo memoria
    *CPUSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), 0);

    handshakeCPUMemoria(CPUSocketMemoria);

    escucharYResponder(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), loger);

    log_destroy(loger);

    procesoCPU->pid = -1;
    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);

    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalDispatch = esperarCliente(&CPUsocketEscuchaDispatch);
    if (*CPUsocketBidireccionalDispatch != -1)
        recibirConn(CPUsocketBidireccionalDispatch, DISPATCH, procesoCPU, interrupcion);
    
    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalInterrupt = esperarCliente(&CPUsocketEscuchaInterrupt);
    if (*CPUsocketBidireccionalInterrupt != -1)
        recibirConn(CPUsocketBidireccionalInterrupt, INTERRUMPT, procesoCPU, interrupcion);
}

void handshakeCPUMemoria(int *CPUSocketMemoria)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, 2);
    enviarMensaje(CPUSocketMemoria, buffer, CPU, HANDSHAKE);

    int resultado = resultadoHandShake(CPUSocketMemoria);

    if (resultado == 1)
    {
        log_info(loger, "Conexión con módulo memoria realizada");
    }
    else
    {
        log_error(loger, "Error de conexión con la memoria");
    }
}

void recibirConn(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);

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

    if (*codigoOperacion == HANDSHAKE)
    {
        enviarPaqueteResult(1, socket, CPU, KERNEL);
        if (conexion == DISPATCH)
        {
            crearHiloDISPATCH(socket, procesoCPU);
        }
        else
        {
            crearHiloINTERRUPT(socket, interrupcion);
        }
    }
    else
    {
        enviarPaqueteResult(-1, socket, CPU, KERNEL);
    }

    free(codigoOperacion);
}

void crearHiloDISPATCH(int *socket, Contexto_proceso *procesoCPU)
{
    pthread_t hiloDISPATCH;

    parametros_hilo_Cpu *params = malloc(sizeof(parametros_hilo_Cpu));
    params->socket = socket;
    params->interrupcion = NULL;
    params->procesoCPU = procesoCPU;

    pthread_create(&hiloDISPATCH,
                   NULL,
                   (void *)manageDISPATCH,
                   params);

    pthread_detach(hiloDISPATCH);
}

void crearHiloINTERRUPT(int *socket, int *interrupcion)
{
    pthread_t hiloINTERRUPT;

    parametros_hilo_Cpu *params = malloc(sizeof(parametros_hilo_Cpu));
    params->socket = socket;
    params->procesoCPU = NULL;
    params->interrupcion = interrupcion;

    pthread_create(&hiloINTERRUPT,
                   NULL,
                   (void *)manageINTERRUPT,
                   params);

    pthread_detach(hiloINTERRUPT);
}