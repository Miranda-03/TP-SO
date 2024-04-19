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
    HandshakeMessageIO h_msg = {STDIN};

    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = 4;
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    memcpy(buffer->stream, &(h_msg.tipoIterfaz), 4);

    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->modulo = IO;
    paquete->buffer = buffer;


    void *a_enviar = malloc(4 + sizeof(uint32_t) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    send(socket, a_enviar, buffer->size + 4 + sizeof(uint32_t), 0);

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

    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
