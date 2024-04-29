#include <Conexion/conectar.h>

int CPUSocketMemoria;

void conectarModuloCPU()
{
    // Conexion con el módulo memoria
    CPUSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);

    handshakeCPUMemoria();

    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int CPUsocketBidireccionalDispatch = esperarCliente(CPUsocketEscuchaDispatch);
    if (CPUsocketBidireccionalDispatch != -1)
        recibirConn(CPUsocketBidireccionalDispatch);

    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int CPUsocketBidireccionalInterrupt = esperarCliente(CPUsocketEscuchaInterrupt);
    if (CPUsocketBidireccionalInterrupt != -1)
        recibirConn(CPUsocketBidireccionalInterrupt);
}

void handshakeCPUMemoria()
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

void recibirConn(int *socket)
{
    TipoModulo *modulo = malloc(sizeof(TipoModulo));
    recv(socket, modulo, sizeof(TipoModulo), MSG_WAITALL);

    switch (*modulo)
    {
    case KERNEL:
        manageKernel(socket);
        break;

    default:
        break;
    }

    free(modulo);
}

void manageKernel(int *socket)
{
    op_code *codigoOperacion = get_opcode_msg_recv(socket);
    printf("LLEGA AL MANAGE KERNEL DEL CPU \n");
    if (*codigoOperacion == HANDSHAKE)
    {
        enviarPaqueteResult(1, socket, CPU, KERNEL);
    }
    else
    {
        // funcion para el kernel
    }

    free(codigoOperacion);
}