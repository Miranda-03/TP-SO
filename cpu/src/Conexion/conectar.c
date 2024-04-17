#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/ModulosEnum.h>

int CPUSocketMemoria;

void conectarModuloCPU()
{
    // Conexion con el módulo memoria
    CPUSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);

    handshakeCPUMemoria();
    /*
    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int CPUsocketBidireccionalDispatch = esperarCliente(CPUsocketEscuchaDispatch);

    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante
    int CPUsocketBidireccionalInterrupt = esperarCliente(CPUsocketEscuchaInterrupt);
    */
}

void handshakeCPUMemoria()
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    t_paquete *paqueteResult = malloc(sizeof(t_paquete));
    t_buffer *bufferResponse = malloc(sizeof(t_buffer));
    t_buffer *buffer = malloc(sizeof(t_buffer));
    // TipoModulo moduloCPU = CPU;
    buffer->size = 4;
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    paquete->modulo = CPU;
    paquete->buffer = buffer;
    paqueteResult->buffer = bufferResponse;

    void *a_enviar = malloc(sizeof(TipoModulo));

    memcpy(a_enviar, &(paquete->modulo), sizeof(TipoModulo));

    send(CPUSocketMemoria, a_enviar, sizeof(TipoModulo), 0);
    recv(CPUSocketMemoria, &(paqueteResult->modulo), sizeof(TipoModulo), 0);
    recv(CPUSocketMemoria, &(paqueteResult->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    void *stream = malloc(sizeof(t_resultHandShake));
    paqueteResult->buffer->stream = stream;
    recv(CPUSocketMemoria, paqueteResult->buffer->stream, sizeof(t_resultHandShake), 0);

    TipoModulo remitente;
    TipoModulo responde;
    uint8_t respuesta;
    memcpy(&remitente, paqueteResult->buffer->stream, sizeof(TipoModulo));
    paqueteResult->buffer->stream += sizeof(TipoModulo);
    memcpy(&responde, paqueteResult->buffer->stream, sizeof(TipoModulo));
    paqueteResult->buffer->stream += sizeof(TipoModulo);
    memcpy(&respuesta, paqueteResult->buffer->stream, sizeof(uint8_t));

    if (respuesta == 0)
    {
        // Handshake OK
        printf("El handshake de CPU a memoria salio bien\n");
    }
    else
    {
        printf("El handshake de CPU a memoria salio mal\n");
        // Handshake ERROR
    }

    free(paquete);
    free(paqueteResult);
    free(bufferResponse);
    free(a_enviar);
    free(stream);
}