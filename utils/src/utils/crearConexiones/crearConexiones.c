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

void enviarPaqueteResult(t_resultHandShake *result, int *result_cod, int *socket)
{
    result->respuesta_cod = result_cod;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_paquete *paquete = malloc(sizeof(t_paquete));

    buffer->size = 8 + sizeof(uint8_t);

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    

    memcpy( buffer->stream + buffer->offset , &(result->moduloRemitente), 4);
    buffer->offset += 4;
    memcpy( buffer->stream + buffer->offset , &(result->moduloResponde), 4);
    buffer->offset += 4;
    memcpy( buffer->stream + buffer->offset , &(result->respuesta_cod), sizeof(uint8_t));

    paquete->modulo = result->moduloResponde; // Podemos usar una constante por operación
    paquete->buffer = buffer;

    void *a_enviar = malloc(buffer->size + 4 + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->modulo), 4);
    offset += 4;
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    send(socket, a_enviar, buffer->size + 4 + sizeof(uint32_t), 0);

    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}