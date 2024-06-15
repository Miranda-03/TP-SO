#include "crearIO.h"

pthread_mutex_t mutexMSGMemoria_io;

void inicializarMutex()
{
    pthread_mutex_init(&mutexMSGMemoria_io, NULL);
}

void crearIO(char *config_file, char *idIO)
{
    // crear el dispositivo impresora a modo de ejemplo
    char *config_path = config_file;
    moduloIO *impresora = instanciar_struct_io(idIO, config_path);
    TipoInterfaz io_interfaz = tipo_interfaz_del_config(config_path);

    int IOsocketKernel;
    int IOsocketMemoria;

    int *IOsocketKernelptr = &IOsocketKernel;
    int *IOsocketMemoriaptr = &IOsocketMemoria;

    conectarModuloIO(io_interfaz, idIO, IOsocketKernelptr, IOsocketMemoriaptr, config_path);

    socket_hilo *sockets = generar_struct_socket_hilo(impresora, IOsocketKernel, IOsocketMemoria, io);

    pthread_t thread;
    pthread_create(&thread, NULL, (void *)hilo_conexion_io, sockets);
    pthread_join(thread);
}

void *hilo_conexion_io(void *ptr)
{
    socket_hilo *sockets = ((socket_hilo *)ptr);
    char *instruccion;
    int io_esta_conectado = 1;
    t_buffer *buffer_kernel;
    int *PID = malloc(sizeof(int));
    t_log *logger = log_create("logs/io.log", "entradasalida", 1, LOG_LEVEL_INFO);

    while (io_esta_conectado)
    {
        //*instruccion = NONE;
        buffer_kernel = recibir_instruccion_del_kernel(instruccion, PID, sockets->IO_Kernel_socket);

        if (strcmp(instruccion, "IO_DISCONNECT") == 0)
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
                manageGenerico(sockets->modulo_io, sockets->IO_Kernel_socket, buffer_kernel, instruccion);
                break;
            case STDIN:
                log_info(logger, mensaje_info_operacion(*PID, "IO_STDIN_READ"));
                manageSTDIN(sockets->modulo_io, sockets->IO_Kernel_socket, sockets->IO_Memoria_socket, buffer_kernel, instruccion);
                break;
            case STDIN:
                log_info(logger, mensaje_info_operacion(*PID, "IO_STDOUT_WRITE"));
                manageSTDOUT(sockets->modulo_io, sockets->IO_Kernel_socket, sockets->IO_Memoria_socket, buffer_kernel, instruccion);
                break
            }
        }
    }
}

/*
    PARA ESCRIBIR LA MEMORIA SE LE ENVIA PRIMERO UN INT: 2 LECTURA, 3 ESCRITURA
*/

void manageSTDIN(moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion)
{
    t_config *config = config_create(modulo_io->config_path);

    int dir_fisica = buffer_read_uint32(buffer_kernel);
    int size_text = buffer_read_uint32(buffer_kernel);

    char texto[size_text] = readline(">");

    int *resultado = -1;

    t_buffer *buffer = buffer_create(8 + strlen(texto) + 1);
    buffer_add_uint32(buffer, 3);
    buffer_add_uint32(buffer, dir_fisica);
    buffer_add_string(buffer, size_text + 1, texto);

    enviarMensajeAMemoria(socketMemoria, texto, dir_fisica, resultado, buffer);

    // Le dice al Kernel que termino con el numero 1
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, *resultado);
    enviarMensaje(socket, buffer, IO, MENSAJE);
    buffer_destroy(buffer_kernel);
    config_destroy(config);
}

void manageSTDOUT(moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion)
{
    t_config *config = config_create(modulo_io->config_path);

    int dir_fisica = buffer_read_uint32(buffer_kernel);
    int size_text = buffer_read_uint32(buffer_kernel);

    t_buffer *buffer = buffer_create(12);
    buffer_add_uint32(buffer, 2);
    buffer_add_uint32(buffer, dir_fisica);
    buffer_add_uint8(buffer, size_text);

    enviarMensaje(socketMemoria, buffer, IO, MENSAJE);

    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *code = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);
    int resultado = buffer_read_uint32(buffer);

    char *texto_resultado = buffer_read_string(buffer, size_text);

    printf("La cadena obtenida es: %s\n", cadena); //HACER LUEGO CON UN LOGER

    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, resultado);
    enviarMensaje(socket, buffer, IO, MENSAJE);
    buffer_destroy(buffer_kernel);
    config_destroy(config);

    buffer_destroy(buffer);
}

void enviarMensajeAMemoria(int *socket, char *texto, int dir_fisica, int *resultado, t_buffer *buffer)
{
    pthread_mutex_lock(&mutexMSGMemoria_io);

    enviarMensaje(socket, buffer, IO, MENSAJE);

    *resultado = esperarResultado(socket);

    pthread_mutex_unlock(&mutexMSGMemoria_io);
}

int esperarResultado(int *socket)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *code = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);
    int resultado = buffer_read_uint32(buffer);

    buffer_destroy(buffer);

    return resultado;
}

void manageGenerico(moduloIO *modulo_io, int *socket, t_buffer *buffer_kernel, char *instruccion)
{
    t_config *config = config_create(modulo_io->config_path);
    int tiempo_unidad;
    int unidades;
    if (config_has_property(config, "TIEMPO_UNIDAD_TRABAJO"))
    {
        tiempo_unidad = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    }

    unidades = atoi(string_split(instruccion, " ")[2]);

    // REALIZA LA OPERACION
    sleep(tiempo_unidad * unidades);

    // Le dice al Kernel que termino con el numero 1
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, 1);
    enviarMensaje(socket, buffer, IO, MENSAJE);
    buffer_destroy(buffer_kernel);
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

t_buffer *recibir_instruccion_del_kernel(char *instruccion, int *PID, int *socket)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *codigo = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);
    //*PID = buffer_read_uint32(buffer); // RECIBIRA ALGUNOS OTROS REGISTROS, NO ES NECESARIO PARA GENERICO. PASAR POR PARAMETRO TIPO DE IO
    instruccion = buffer_read_string(buffer);

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