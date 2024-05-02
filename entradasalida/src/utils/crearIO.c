#include "crearIO.h"

void crearIO()
{
    // crear el dispositivo impresora a modo de ejemplo
    char *config_path = "io_config/impresora.config";
    moduloIO *impresora = instanciar_struct_io("impresora", config_path);
    TipoInterfaz *io_interfaz = tipo_interfaz_del_config(config_path);

    int *IOsocketKernel = malloc(sizeof(int));
    int *IOsocketMemoria = malloc(sizeof(int));
    *IOsocketMemoria = NULL;
    conectarModuloIO(*io_interfaz, "impresora", IOsocketKernel, IOsocketMemoria);
    socket_hilo *sockets = malloc(sizeof(socket_hilo));
    sockets->IO_Kernel_socket = IOsocketKernel;
    sockets->IO_Memoria_socket = IOsocketMemoria;
    sockets->modulo_io = impresora;
    sockets->tipo_interfaz = *io_interfaz;
    pthread_t thread;
    pthread_create(&thread, NULL, (void *)hilo_conexion_io, sockets);
    pthread_detach(thread);
    free(io_interfaz);
}

void *hilo_conexion_io(void *ptr)
{
    socket_hilo *sockets = *((socket_hilo *)ptr);
    instruccionIO *instruccion = malloc(sizeof(instruccionIO));
    int io_esta_conectado = 1;

    while (io_esta_conectado)
    {
        *instruccion = NULL;
        do
        {
            recv(sockets->IO_Kernel_socket, instruccion, sizeof(instruccionIO), 0);
        } while (*instruccion == NULL);

        if (*instruccion == IO_DISCONNECT)
        {
            free(sockets->IO_Kernel_socket);
            free(sockets->IO_Memoria_socket);
            free(sockets->modulo_io);
            free(sockets);
            free(instruccion);
            io_esta_conectado = 0;
        }
        else
        {
            switch (sockets->tipo_interfaz)
            {
            case GENERICA:
                manageGenerico(sockets->modulo_io, sockets->IO_Kernel_socket);
                break;
            }
        }
    }
}

void manageGenerico(moduloIO *modulo_io, int *socket)
{
    t_config *config = config_create(modulo_io->config_path);
    int tiempo_unidad;
    int unidades;
    if (config_has_property(config, "TIEMPO_UNIDAD_TRABAJO"))
    {
        tiempo_unidad = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    }
    recv(socket, &unidades, sizeof(uint32_t), 0);
    
    //Realiza la operacion 
    sleep(tiempo_unidad * unidades);

    //Le dice al Kernel que termino con el numero 1
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, 1);
    enviarMensaje(socket, buffer, IO, MENSAJE);
    config_destroy(config);
    buffer_destroy(buffer);
}

moduloIO *instanciar_struct_io(char *identificador, char *config_path)
{
    moduloIO *io = malloc(sizeof(moduloIO));
    io->identificador = identificador;
    io->config_path = config_path;
    return io;
}

TipoInterfaz *tipo_interfaz_del_config(char *config_path)
{
    t_config *IOconfig = config_create(char *config_path);
    TipoInterfaz *io_interfaz = config_get_int_value(IOconfig *, "TIPO_INTERFAZ");
    config_destroy(IOconfig);
    return io_interfaz;
}