#include "crearIO.h"

pthread_mutex_t mutexMSGMemoria_io;

void inicializarMutex()
{
    pthread_mutex_init(&mutexMSGMemoria_io, NULL);
}

void crearIO(char *config_file, char *idIO, pthread_t *hilo_de_escucha)
{
    // crear el dispositivo impresora a modo de ejemplo
    char *identificador = string_split(idIO, ".")[0];
    moduloIO *impresora = instanciar_struct_io(identificador, config_file);
    TipoInterfaz io_interfaz = tipo_interfaz_del_config(config_file);

    int IOsocketKernel;
    int IOsocketMemoria;

    int *IOsocketKernelptr = &IOsocketKernel;
    int *IOsocketMemoriaptr = &IOsocketMemoria;

    conectarModuloIO(io_interfaz, identificador, IOsocketKernelptr, IOsocketMemoriaptr, config_file);

    socket_hilo *sockets = generar_struct_socket_hilo(impresora, &IOsocketKernel, &IOsocketMemoria, io_interfaz);

    pthread_create(hilo_de_escucha, NULL, (void *)hilo_conexion_io, sockets);
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
        buffer_kernel = recibir_instruccion_del_kernel(&instruccion, PID, &(sockets->IO_Kernel_socket));

        if (strcmp(instruccion, "IO_DISCONNECT") == 0)
        {
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
                manageGenerico(sockets->modulo_io, &(sockets->IO_Kernel_socket), buffer_kernel, instruccion);
                break;
            case STDIN:
                log_info(logger, mensaje_info_operacion(*PID, "IO_STDIN_READ"));
                manageSTDIN(sockets->modulo_io, &(sockets->IO_Kernel_socket), &(sockets->IO_Memoria_socket), buffer_kernel, instruccion, *PID);
                break;
            case STDOUT:
                log_info(logger, mensaje_info_operacion(*PID, "IO_STDOUT_WRITE"));
                manageSTDOUT(sockets->modulo_io, &(sockets->IO_Kernel_socket), &(sockets->IO_Memoria_socket), buffer_kernel, instruccion, *PID);
                break;
            }
        }
    }
}

/*
    PARA ESCRIBIR LA MEMORIA SE LE ENVIA PRIMERO UN INT: 2 LECTURA, 3 ESCRITURA
*/

void manageSTDIN(moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion, int pid)
{
    char *texto;

    char *linea_comando = malloc(strlen(modulo_io->identificador) + 3);

    strcpy(linea_comando, modulo_io->identificador);
    strcat(linea_comando, ">");

    texto = readline(linea_comando);

    free(linea_comando);

    char **instruccionSeparada = string_split(instruccion, " ");

    char **direcciones_fisicas = string_get_string_as_array(instruccionSeparada[2]);

    int len_dr_fisicas = string_array_size(direcciones_fisicas);

    int offset = 0;

    int resultado = 0;

    for (int i = 0; i < (len_dr_fisicas / 2); i++)
    {
        t_buffer *buffer = buffer_create(4 + 4 + 4 + atoi(direcciones_fisicas[i + 1]));
        buffer_add_uint32(buffer, pid);
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i]));
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i + 1]));
        buffer_add(buffer, texto + offset, atoi(direcciones_fisicas[i + 1]));

        offset += atoi(direcciones_fisicas[i + 1]);

        enviarMensaje(socketMemoria, buffer, IO, ESCRIBIR_MEMORIA);

        TipoModulo *modulo = get_modulo_msg_recv(socketMemoria);
        op_code *opcode = get_opcode_msg_recv(socketMemoria);
        t_buffer *buffer_recv = buffer_leer_recv(socketMemoria);

        resultado = buffer_read_uint32(buffer_recv);

        if (resultado < 0)
            break;

        buffer_destroy(buffer_recv);
    }

    // Le dice al Kernel que termino con el numero 1
    t_buffer *buffer_respuesta_kernel = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer_respuesta_kernel, resultado);
    enviarMensaje(socket, buffer_respuesta_kernel, IO, MENSAJE);
    buffer_destroy(buffer_kernel);
    free(texto);
}

void manageSTDOUT(moduloIO *modulo_io, int *socket, int *socketMemoria, t_buffer *buffer_kernel, char *instruccion, int pid)
{
    char **instruccionSeparada = string_split(instruccion, " ");
    
    char **direcciones_fisicas = string_get_string_as_array(instruccionSeparada[2]);

    int len_dr_fisicas = string_array_size(direcciones_fisicas);

    char *dato_a_leer = string_new();

    for (int i = 0; i < (len_dr_fisicas / 2); i++)
    {
        t_buffer *buffer = buffer_create(12);
        buffer_add_uint32(buffer, pid);
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i]));
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i + 1]));

        enviarMensaje(socketMemoria, buffer, IO, LEER_MEMORIA);

        TipoModulo *modulo = get_modulo_msg_recv(socketMemoria);
        op_code *opcode = get_opcode_msg_recv(socketMemoria);
        t_buffer *buffer_recv = buffer_leer_recv(socketMemoria);

        string_append(&dato_a_leer, buffer_read_string(buffer_recv, atoi(direcciones_fisicas[i + 1])));

        buffer_destroy(buffer_recv);
    }

    printf("%s\n", dato_a_leer);

    t_buffer *buffer_respuesta_para_kernel = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer_respuesta_para_kernel, 1);
    enviarMensaje(socket, buffer_respuesta_para_kernel, IO, MENSAJE);
    buffer_destroy(buffer_kernel);
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
    TipoInterfaz io_interfaz = tipo_interfaz_config(config_path);
    return io_interfaz;
}

TipoInterfaz tipo_interfaz_config(char *config_path)
{
    char *tipo = obtenerValorConfig(config_path, "TIPO_INTERFAZ");

    if (strcmp(tipo, "GENERICA") == 0)
        return GENERICA;
    if (strcmp(tipo, "STDIN") == 0)
        return STDIN;
    if (strcmp(tipo, "STDOUT") == 0)
        return STDOUT;
    if (strcmp(tipo, "DIALFS") == 0)
        return DIALFS;
}

socket_hilo *generar_struct_socket_hilo(moduloIO *modulo_io, int *IOsocketKernel, int *IOsocketMemoria, TipoInterfaz interfaz)
{
    socket_hilo *io_hilo = malloc(sizeof(socket_hilo));
    io_hilo->modulo_io = modulo_io;
    io_hilo->IO_Kernel_socket = *IOsocketKernel;
    io_hilo->IO_Memoria_socket = *IOsocketMemoria;
    io_hilo->tipo_interfaz = interfaz;

    return io_hilo;
}

t_buffer *recibir_instruccion_del_kernel(char **instruccion, int *PID, int *socket)
{
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *codigo = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);
    *PID = buffer_read_uint32(buffer); // RECIBIRA ALGUNOS OTROS REGISTROS, NO ES NECESARIO PARA GENERICO. PASAR POR PARAMETRO TIPO DE IO
    int size = buffer_read_uint32(buffer);
    *instruccion = buffer_read_string(buffer, size);
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