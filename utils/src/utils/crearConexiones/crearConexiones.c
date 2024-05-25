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

void enviarPaqueteResult(int result_cod, int *socket, TipoModulo moduloResponde, TipoModulo moduloRemitente)
{
    printf("result_code %d\n", result_cod);
    t_buffer *buffer = buffer_create(sizeof(TipoModulo) * 2 + sizeof(int));
    buffer_add_uint32(buffer, 4);

    enviarMensaje(socket, buffer, moduloResponde, MENSAJE);
}

int resultadoHandShake(int *socket)
{

    op_code *op_code = get_opcode_msg_recv(socket);
    TipoModulo *modulo = get_modulo_msg_recv(socket);
    void *stream = buffer_leer_stream_recv(socket);
    uint32_t *respuesta = malloc(sizeof(uint32_t));
    memcpy(respuesta, stream, sizeof(uint32_t));
    printf("respuesta del resultadoHandShake del stream %d\n", *respuesta);
    if (*respuesta == 1)
    {
        // Handshake OK
        printf("El handshake salio bien\n");
        free(respuesta);
        return 1;
    }
    else
    {
        printf("El handshake salio mal\n");
        free(respuesta);
        return 0;
        // Handshake ERROR
    }
}