#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/structs/structSendRecvMSG.h>

typedef struct
{
    TipoInterfaz tipoIterfaz;
} HandshakeMessageIO;

int IOsocketKernel;
int IOsocketMemoria;

void conectarModuloIO(int *esGenerico)
{
    IOsocketKernel = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_KERNEL"), obtenerValorConfig(PATH_CONFIG, "IP_KERNEL"), NULL);
    realizarHandshakeIO(IOsocketKernel);

    if (esGenerico != 1)
    {
        IOsocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
        realizarHandshakeIO(IOsocketMemoria);
    }
}

void realizarHandshakeIO(int *socket)
{
    t_buffer *buffer = buffer_create(sizeof(TipoInterfaz));
    TipoInterfaz tipo = STDIN;
    buffer_add(buffer, &tipo, 4);
    //buffer_add_uint32(buffer, STDIN);
    enviarMensaje(socket, buffer, IO, HANDSHAKE);

    int respuestaHandshake = resultadoHandShake(socket);

    if (respuestaHandshake == 1)
    {
        // Handshake OK
        printf("El handshake salio bien\n");
    }
    else
    {
        printf("El handshake salio mal\n");
        // Handshake ERROR
    }
}
