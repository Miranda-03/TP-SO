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

void conectarModuloIO()
{
    // IOsocketKernel = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_KERNEL"), obtenerValorConfig(PATH_CONFIG, "IP_KERNEL"), NULL);
    IOsocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);

    realizarHandshakeIO();
}

void realizarHandshakeIO()
{
    printf("Entra al handshake IO\n");
    HandshakeMessageIO h_msg = {GENERICA};

    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = 4;
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    memcpy(buffer->stream, &(h_msg.tipoIterfaz), 4);

    t_paquete *paquete = malloc(sizeof(t_paquete));
    t_paquete *paqueteResult = malloc(sizeof(t_paquete));

    paquete->modulo = IO;
    paquete->buffer = buffer;

    void *a_enviar = malloc(4 + 4 + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->modulo), sizeof(TipoModulo));

    offset += 4;
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    send(IOsocketMemoria, a_enviar, buffer->size + 4 + sizeof(uint32_t), 0);

    recv(IOsocketMemoria, &(paqueteResult->modulo), 4, MSG_WAITALL);
    recv(IOsocketMemoria, &(paqueteResult->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    paqueteResult->buffer->stream = malloc(paqueteResult->buffer->size);
    recv(IOsocketMemoria, paqueteResult->buffer->stream, paqueteResult->buffer->size, MSG_WAITALL);

    TipoModulo remitente;
    TipoModulo responde;
    uint8_t respuesta;
    memcpy(&remitente, paqueteResult->buffer->stream, 4);
    paqueteResult->buffer->stream += 4;
    memcpy(&responde, paqueteResult->buffer->stream, 4);
    paqueteResult->buffer->stream += 4;
    memcpy(&respuesta, paqueteResult->buffer->stream, sizeof(uint8_t));

    if (respuesta == 0)
    {
        // Handshake OK
        printf("El handshake salio bien\n");
    }
    else
    {
        printf("El handshake salio mal\n");
        // Handshake ERROR
    }

    printf("llega al final IO\n");

    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
    free(paqueteResult);
}