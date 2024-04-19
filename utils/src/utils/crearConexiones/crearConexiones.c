#include "crearConexiones.h"

int crearSocket(char *puerto, char *ip, int *MaxConexiones)
{
    int resultSocket;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(ip, puerto, &hints, &servinfo);
    if (status != 0)
    {
        fprintf(stderr, "Error al obtener la dirección del servidor: %s\n", gai_strerror(status));
    }

    resultSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if (ip == NULL)
        sockerModoEscucha(resultSocket, servinfo, MaxConexiones);
    else
        conectarServidor(resultSocket, servinfo);

    freeaddrinfo(servinfo);

    return resultSocket;
}

void sockerModoEscucha(int *socket, struct addrinfo *servinfo, int *MaxConexiones)
{
    bind(socket, servinfo->ai_addr, servinfo->ai_addrlen);
    listen(socket, MaxConexiones);
}

void conectarServidor(int *socket, struct addrinfo *servinfo)
{
    int err = 0;
    do
    {
        err = connect(socket, servinfo->ai_addr, servinfo->ai_addrlen);
    } while (err != -1);
}

int esperarCliente(int *socket)
{
    return accept(socket, NULL, NULL);
}

void enviarPaqueteResult(t_resultHandShake *result, int result_cod, int *socket)
{
    result->respuesta_cod = result_cod;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_paquete *paquete = malloc(sizeof(t_paquete));

    buffer->size = sizeof(t_resultHandShake);
    
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    memcpy(buffer->stream + buffer->offset, &(result->moduloRemitente), sizeof(TipoModulo));
    buffer->offset += sizeof(TipoModulo);
    memcpy(buffer->stream + buffer->offset, &(result->moduloResponde), sizeof(TipoModulo));
    buffer->offset += sizeof(TipoModulo);
    memcpy(buffer->stream + buffer->offset, &(result->respuesta_cod), sizeof(uint8_t));

    paquete->modulo = result->moduloResponde;
    paquete->buffer = buffer;

    void *a_enviar = malloc(buffer->size + sizeof(TipoModulo) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    send(socket, a_enviar, buffer->size + 4 + sizeof(uint32_t), 0);

    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

int resultadoHandShake(int *socket)
{

    t_buffer *bufferResponse = malloc(sizeof(t_buffer));
    t_paquete *paqueteResult = malloc(sizeof(t_paquete));

    paqueteResult->buffer = bufferResponse;

    recv(socket, &(paqueteResult->modulo), 4, 0);
    recv(socket, &(paqueteResult->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    void *stream = malloc(sizeof(t_resultHandShake));
    paqueteResult->buffer->stream = stream;
    recv(socket, paqueteResult->buffer->stream, sizeof(t_resultHandShake), 0);

    TipoModulo remitente;
    TipoModulo responde;
    uint8_t respuesta;
    memcpy(&remitente, paqueteResult->buffer->stream, sizeof(TipoModulo));
    paqueteResult->buffer->stream += sizeof(TipoModulo);
    memcpy(&responde, paqueteResult->buffer->stream, sizeof(TipoModulo));
    paqueteResult->buffer->stream += sizeof(TipoModulo);
    memcpy(&respuesta, paqueteResult->buffer->stream, sizeof(uint8_t));

    free(bufferResponse);
    free(paqueteResult);
    free(stream);


    if (respuesta == 1)
    {
        // Handshake OK
        printf("El handshake salio bien\n");
        return 1;
    }
    else
    {
        printf("El handshake salio mal\n");
        return 0;
        // Handshake ERROR
    }
}