#include "crearIO.h"

void crearIO()
{
    // crear el dispositivo impresora a modo de ejemplo
    char *config_path = "io_config/impresora.config";
    moduloIO *impresora = instanciar_struct_io("impresora", config_path);
    TipoInterfaz io_interfaz = tipo_interfaz_del_config(config_path);

    int IOsocketKernel;
    int IOsocketMemoria;

    int *IOsocketKernelptr = &IOsocketKernel;
    int *IOsocketMemoriaptr = &IOsocketMemoria;

    conectarModuloIO(io_interfaz, "impresora", IOsocketKernelptr, IOsocketMemoriaptr);

    socket_hilo *sockets = generar_struct_socket_hilo(impresora, IOsocketKernel, IOsocketMemoria, io);

    pthread_t thread;
    pthread_create(&thread, NULL, (void *)hilo_conexion_io, sockets);
    pthread_join(thread);
}

void *hilo_conexion_io(void *ptr)
{
    socket_hilo *sockets = ((socket_hilo *)ptr);
    instruccionIO *instruccion = malloc(sizeof(instruccionIO));
    int io_esta_conectado = 1;
    t_buffer *buffer_kernel;
    int *PID = malloc(sizeof(int));
    t_log *logger = log_create("logs/io.log", "entradasalida", 1, LOG_LEVEL_INFO);

    while (io_esta_conectado)
    {
        *instruccion = NONE;
        buffer_kernel = recibir_instruccion_del_kernel(instruccion, PID, sockets->IO_Kernel_socket);

        if (*instruccion == IO_DISCONNECT)
        {
            free(sockets->IO_Kernel_socket);
            free(sockets->IO_Memoria_socket);
            destruir_struct_io(sockets->modulo_io);
            free(sockets);
            free(instruccion);
            log_destroy(logger);
            io_esta_conectado = 0;
        }
        else
        {
            switch (sockets->tipo_interfaz)
            {
            case GENERICA:

                log_info(logger, mensaje_info_operacion(*PID, "IO_SLEEP_GEN"));
                manageGenerico(sockets->modulo_io, sockets->IO_Kernel_socket, buffer_kernel);
                break;
            }
        }
    }
}

void manageGenerico(moduloIO *modulo_io, int *socket, t_buffer *buffer_kernel)
{
    t_config *config = config_create(modulo_io->config_path);
    int tiempo_unidad;
    int unidades;
    if (config_has_property(config, "TIEMPO_UNIDAD_TRABAJO"))
    {
        tiempo_unidad = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    }

    unidades = buffer_read_uint32(buffer_kernel);

    // REALIZA LA OPERACION
    sleep(tiempo_unidad * unidades);

    // Le dice al Kernel que termino con el numero 1
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, 1);
    enviarMensaje(socket, buffer, IO, MENSAJE);
    config_destroy(config);
}

moduloIO *instanciar_struct_io(const char *identificador, const char *config_path)
{
    moduloIO *io = malloc(sizeof(moduloIO));
    if (io == NULL)
    {
        // Manejo de error de asignación de memoria
        return NULL;
    }

    // Copiar el identificador
    io->identificador = strdup(identificador);
    if (io->identificador == NULL)
    {
        // Manejo de error de asignación de memoria
        free(io);
        return NULL;
    }

    // Copiar el config_path
    io->config_path = strdup(config_path);
    if (io->config_path == NULL)
    {
        // Manejo de error de asignación de memoria
        free(io->identificador);
        free(io);
        return NULL;
    }

    return io;
}

void destruir_struct_io(moduloIO *struct_io)
{
    free(struct_io->config_path);
    free(struct_io->identificador);
    free(struct_io);
}

TipoInterfaz tipo_interfaz_del_config(char *config_path)
{
    t_config *IOconfig = config_create(config_path);
    TipoInterfaz io_interfaz = config_get_int_value(IOconfig, "TIPO_INTERFAZ");
    config_destroy(IOconfig);
    return io_interfaz;
}

socket_hilo *generar_struct_socket_hilo(moduloIO *modulo_io, int *IOsocketKernel, int *IOsocketMemoria, TipoInterfaz interfaz)
{
    socket_hilo *io_hilo = malloc(sizeof(socket_hilo));
    io_hilo->modulo_io = modulo_io;
    io_hilo->IO_Kernel_socket = IOsocketKernel;
    io_hilo->IO_Memoria_socket = IOsocketMemoria;
    io_hilo->tipo_interfaz = interfaz;

    return io_hilo;
}

t_buffer *recibir_instruccion_del_kernel(instruccionIO *instruccion, int *PID, int *socket)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *codigo = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);
    *PID = buffer_read_uint32(buffer);
    *instruccion = buffer_read_uint32(socket);

    return buffer;
}

char *mensaje_info_operacion(int PID, char *operacion)
{
    char *result = string_new();
    string_append(&result, "PID: ");
    string_append(&result, string_itoa(PID));
    string_append(&result, " - Operacion: ");
    string_append(&result, operacion);
    return result;
}