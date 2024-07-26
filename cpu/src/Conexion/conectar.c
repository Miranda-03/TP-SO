#include <Conexion/conectar.h>

t_log *loger;

void conectarModuloCPU(int *CPUSocketMemoria, int *CPUsocketBidireccionalDispatch, int *CPUsocketBidireccionalInterrupt, Contexto_proceso *procesoCPU, int *interrupcion, char *path_config)
{
    loger = log_create("logs/cpu.log", "cpu_conn", 1, LOG_LEVEL_INFO);

    //Obtener direccion IP del modulo memoria
    t_config *cpu_config = config_create(path_config);
    solicitar_ip("255.255.255.255", config_get_string_value(cpu_config, "PUERTO_MEMORIA"), cpu_config, "IP_MEMORIA", loger, "SOLICITAR_IP");
    config_destroy(cpu_config);

    // Conexion con el módulo memoria
    *CPUSocketMemoria = crearSocket(obtenerValorConfig(path_config, "PUERTO_MEMORIA"), obtenerValorConfig(path_config, "IP_MEMORIA"), 0);
    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(path_config, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(path_config, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);

    handshakeCPUMemoria(CPUSocketMemoria);

    escucharYResponder(obtenerValorConfig(path_config, "PUERTO_ESCUCHA_DISPATCH"), loger, NULL, 0);

    log_destroy(loger);

    procesoCPU->pid = -1;

    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalDispatch = esperarCliente(&CPUsocketEscuchaDispatch);
    if (*CPUsocketBidireccionalDispatch != -1)
        recibirConn(CPUsocketBidireccionalDispatch, DISPATCH, procesoCPU, interrupcion, path_config);
    
    // la siguiente linea es autobloqueante
    *CPUsocketBidireccionalInterrupt = esperarCliente(&CPUsocketEscuchaInterrupt);
    if (*CPUsocketBidireccionalInterrupt != -1)
        recibirConn(CPUsocketBidireccionalInterrupt, INTERRUMPT, procesoCPU, interrupcion, path_config);
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

void recibirConn(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion, char *path_config)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);

    switch (*modulo)
    {
    case KERNEL:
        manageKernel(socket, conexion, procesoCPU, interrupcion, path_config);
        break;

    default:
        break;
    }

    free(modulo);
}

void manageKernel(int *socket, TipoConn conexion, Contexto_proceso *procesoCPU, int *interrupcion, char *path_config)
{
    op_code *codigoOperacion = get_opcode_msg_recv(socket);

    if (*codigoOperacion == HANDSHAKE)
    {
        enviarPaqueteResult(1, socket, CPU, KERNEL);
        if (conexion == DISPATCH)
        {
            crearHiloDISPATCH(socket, procesoCPU, path_config);
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

void crearHiloDISPATCH(int *socket, Contexto_proceso *procesoCPU, char *path_config)
{
    pthread_t hiloDISPATCH;

    parametros_hilo_Cpu *params = malloc(sizeof(parametros_hilo_Cpu));
    params->socket = socket;
    params->interrupcion = NULL;
    params->procesoCPU = procesoCPU;
    params->path_config = path_config;

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
    params->path_config = NULL;

    pthread_create(&hiloINTERRUPT,
                   NULL,
                   (void *)manageINTERRUPT,
                   params);

    pthread_detach(hiloINTERRUPT);
}