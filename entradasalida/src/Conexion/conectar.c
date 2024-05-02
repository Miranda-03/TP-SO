#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/structs/structSendRecvMSG.h>

typedef struct
{
    TipoInterfaz tipoIterfaz;
} HandshakeMessageIO;


void conectarModuloIO(TipoInterfaz tipo_interfaz, char* identificador, int *IOsocketKernel, int *IOsocketMemoria)
{
    IOsocketKernel = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_KERNEL"), obtenerValorConfig(PATH_CONFIG, "IP_KERNEL"), NULL);
    realizarHandshakeIO(tipo_interfaz, identificador, IOsocketKernel);

    if (esGenerico != 1)
    {
        IOsocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
        realizarHandshakeIO(tipo_interfaz, identificador, IOsocketMemoria);
    }
}

void realizarHandshakeIO(TipoInterfaz tipo_interfaz, char* identificador, int *socket)
{
    t_buffer *buffer = buffer_create(sizeof(TipoInterfaz) + strlen(identificador) + 1 + sizeof(uint32_t));
    buffer_add(buffer, &tipo_interfaz, 4);
    buffer_add_uint32(buffer, strlen(identificador) + 1);
    buffer_add_string(buffer, strlen(identificador) + 1, identificador);
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
