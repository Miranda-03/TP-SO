#include "Conexion/conectar.h"

typedef struct
{
    int socket;
    t_dictionary *interfaces_conectadas;
} parametros_hilo_IO_Kernel;

t_log *loger;

char *path_config_conn;

void conectarModuloKernel(int *KernelSocketMemoria, int *KernelSocketCPUDispatch, int *KernelSocketCPUInterrumpt, t_dictionary *interfaces_conectadas, char *path_config)
{
    loger = log_create("logs/kernel_conn.log", "kernel_conn", 1, LOG_LEVEL_INFO);
    path_config_conn = path_config;
    t_config *config = config_create(path_config);

    solicitar_ip("255.255.255.255", config_get_string_value(config, "PUERTO_CPU_DISPATCH"), config, "IP_CPU", loger, "SOLICITAR_IP");

    // Conexiones con el módulo CPU
    *KernelSocketCPUDispatch = crearSocket(config_get_string_value(config, "PUERTO_CPU_DISPATCH"), config_get_string_value(config, "IP_CPU"), 0);
    handshakeKernelCPU(DISPATCH, KernelSocketCPUDispatch);

    *KernelSocketCPUInterrumpt = crearSocket(config_get_string_value(config, "PUERTO_CPU_INTERRUPT"), config_get_string_value(config, "IP_CPU"), 0);
    handshakeKernelCPU(INTERRUMPT, KernelSocketCPUInterrumpt);

    obtener_ip_de_modulo_memoria(*KernelSocketCPUDispatch, config);

    // PUERTO_MEMORIA, IP_MEMORIA
    *KernelSocketMemoria = crearSocket(config_get_string_value(config, "PUERTO_MEMORIA"), config_get_string_value(config, "IP_MEMORIA"), 0);
    handshakeKernelMemoria(KernelSocketMemoria);

    // Recibir conexiones de IO
    int KernelsocketEscucha = crearSocket(config_get_string_value(config, "PUERTO_ESCUCHA"), NULL, MAXCONN);
    parametros_hilo_IO_Kernel *params = malloc(sizeof(parametros_hilo_IO_Kernel));
    params->socket = KernelsocketEscucha;
    params->interfaces_conectadas = interfaces_conectadas;
    inicializarMutexDiccionarioIOConectadas();
    pthread_t threadClientes;
    pthread_create(&threadClientes, NULL, recibirClientes, (void *)params);
    pthread_detach(threadClientes);

    config_destroy(config);
}

void *recibirClientes(void *ptr)
{
    parametros_hilo_IO_Kernel *params = (parametros_hilo_IO_Kernel *)ptr;

    pthread_t hilo_escucha_broadcast;
    pthread_create(&hilo_escucha_broadcast, NULL, hilo_responder_ips, NULL);
    pthread_detach(hilo_escucha_broadcast);

    int socketEscucha = params->socket;
    while (1)
    {
        pthread_t thread;

        int *socketBidireccional = malloc(sizeof(int));
        *socketBidireccional = accept(socketEscucha, NULL, NULL);
        if (socketBidireccional < 0)
        {
            perror("accept");
            free(socketBidireccional);
            continue;
        }

        params->socket = *socketBidireccional;
        pthread_create(&thread, NULL, atenderIO, (void *)params); // Pasar el descriptor de socket como un puntero
        pthread_join(thread, NULL);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderIO(void *ptr)
{
    parametros_hilo_IO_Kernel *params = (parametros_hilo_IO_Kernel *)ptr; // Castear correctamente el descriptor de socket
    int socket = params->socket;

    TipoModulo *modulo = get_modulo_msg_recv(&socket);
    op_code *codigoOperacion = get_opcode_msg_recv(&socket);

    if (*codigoOperacion == MENSAJE && *modulo == IO)
    {
        t_buffer *buffer = buffer_leer_recv(&socket);
        TipoInterfaz interfaz = buffer_read_uint32(buffer);
        int sizeIdentificador = buffer_read_uint32(buffer);
        char *identificador = buffer_read_string(buffer, sizeIdentificador);

        guardar_interfaz_conectada(&socket, interfaz, identificador, params->interfaces_conectadas);
        buffer_destroy(buffer);

        enviarPaqueteResult(1, &socket, IO, KERNEL);
    }
    else
    {
        enviarPaqueteResult(-1, &socket, IO, KERNEL);
    }
    free(codigoOperacion);
    free(modulo);
    pthread_exit(NULL);
}

void handshakeKernelCPU(TipoConn conn, int *socket)
{
    t_buffer *buffer = buffer_create(sizeof(TipoConn));

    if (!buffer)
    {
        perror("buffer_create");
        return;
    }
    buffer_add_uint32(buffer, conn);

    enviarMensaje(socket, buffer, KERNEL, HANDSHAKE);

    char *tipo_conn = get_tipo_conn(conn);

    char *mensaje_loger = string_new();

    if (resultadoHandShake(socket) == 1)
    {
        string_append(&mensaje_loger, "Se establece conexion con CPU: ");
        string_append(&mensaje_loger, tipo_conn);
        log_info(loger, mensaje_loger);
    }
    else
    {
        string_append(&mensaje_loger, "Error de conexion con CPU: ");
        string_append(&mensaje_loger, tipo_conn);
        log_error(loger, mensaje_loger);
    }

    free(mensaje_loger);
}

char *get_tipo_conn(TipoConn conn)
{
    if (conn == DISPATCH)
        return "DISPATCH";
    return "INTERRUMPT";
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
        log_info(loger, "Se establece conexion con la memoria");
    else
        log_error(loger, "Error en la conexion con la memoria");
}

void obtener_ip_de_modulo_memoria(int socket, t_config *config)
{
    enviarMensaje(&socket, NULL, KERNEL, OBTENER_IP_MEMORIA);

    TipoModulo *modulo = get_modulo_msg_recv(&socket);
    op_code *opcode = get_opcode_msg_recv(&socket);

    t_buffer *buffer = buffer_leer_recv(&socket);

    int size = buffer_read_uint32(buffer);
    char *ip = buffer_read_string(buffer, size);

    config_set_value(config, "IP_MEMORIA", ip);
    config_save(config);
    free(modulo);
    free(opcode);
}

void *hilo_responder_ips(void *ptr)
{
    t_config *config = config_create(path_config_conn);
    escucharYResponder(config_get_string_value(config, "PUERTO_ESCUCHA"), loger, config_get_string_value(config, "IP_MEMORIA"), 1);
}