#include <Conexion/conectar.h>

void conectarModuloMemoria()
{
    pthread_t threadModulos;

    int MemoriasocketEscucha = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA"), NULL, 10);

    pthread_create(&threadModulos, NULL, recibirModulo, (void *)&MemoriasocketEscucha);
    pthread_join(threadModulos, NULL);
}

void *recibirModulo(void *ptr)
{
    int *MemoriasocketEscucha = (int *)ptr; // Castear correctamente el descriptor de socket
    while (1)
    {
        pthread_t thread;
        int *socketBidireccional = malloc(sizeof(int));

        *socketBidireccional = accept(*MemoriasocketEscucha, NULL, NULL);

        pthread_create(&thread,
                       NULL,
                       (void *)atenderModulo,
                       socketBidireccional); // Pasar el descriptor de socket como un puntero
        pthread_detach(thread);
    }
    return NULL; // Agregar un return al final de la función
}

void *atenderModulo(void *ptr)
{

    int *socketComunicacion = ((int *)ptr);
    TipoModulo moduloRemitente;
    recv(*socketComunicacion, &moduloRemitente, sizeof(TipoModulo), 0);

    switch (moduloRemitente)
    {
    case IO:
        manageIO(socketComunicacion);
        break;

    default:
        manageModulo(socketComunicacion, moduloRemitente);
        break;
    }
    pthread_exit(NULL);
}

void manageModulo(int *socket, TipoModulo modulo)
{
    op_code *codigoOperacion = get_opcode_msg_recv(socket);

    if (*codigoOperacion == HANDSHAKE)
    {

        enviarPaqueteResult(1, socket, MEMORIA, modulo);
        iniciar_hilo_conexion(socket, modulo);
    }
    else
    {
        enviarPaqueteResult(-1, socket, MEMORIA, modulo);
    }

    free(codigoOperacion);
}

void manageIO(int *socket)
{
    op_code *codigoOperacion = get_opcode_msg_recv(socket);

    t_buffer *buffer = buffer_leer_recv(socket);
    TipoInterfaz tipo = buffer_read_uint32(buffer);

    free(buffer); // no eliminar el buffer para luego usar el resto del stream

    switch (tipo) // primero fijarse el codigo de operacion en cada 'case' del switch
    {
    case STDIN:
        enviarPaqueteResult(1, socket, MEMORIA, IO);
        crearHiloManageSTDIN(socket);
        break;

    case STDOUT:
        enviarPaqueteResult(1, socket, MEMORIA, IO);
        crearHiloManageSTDOUT(socket);
        break;

    case DIALFS:
        enviarPaqueteResult(1, socket, MEMORIA, IO);
        crearHiloManageDIALFS(socket);
        break;

    default:
        enviarPaqueteResult(-1, socket, MEMORIA, IO);
        break;
    }
}

void crearHiloManageSTDIN(int *socket)
{
    pthread_t hilo_manage_stdin;
    pthread_create(&hilo_manage_stdin,
                   NULL,
                   (void *)manage_conn_stdin_io,
                   socket);
    pthread_detach(hilo_manage_stdin);
}

void crearHiloManageSTDOUT(int *socket)
{
    pthread_t hilo_manage_stdout;
    pthread_create(&hilo_manage_stdout,
                   NULL,
                   (void *)manage_conn_stdout_io,
                   socket);
    pthread_detach(hilo_manage_stdout);
}

void crearHiloManageDIALFS(int *socket)
{
    pthread_t hilo_manage_dial;
    pthread_create(&hilo_manage_dial,
                   NULL,
                   (void *)manage_conn_dialFS_io,
                   socket);
    pthread_detach(hilo_manage_dial);
}

void iniciar_hilo_conexion(int *socket, TipoModulo modulo)
{
    pthread_t thread_conexion_cpu;
    if (modulo == CPU)
        pthread_create(&thread_conexion_cpu,
                       NULL,
                       (void *)manage_conn_cpu,
                       socket);
    else
        pthread_create(&thread_conexion_cpu,
                       NULL,
                       (void *)manage_conn_kernel,
                       socket);
    pthread_detach(thread_conexion_cpu);
}