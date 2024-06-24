#include "crearConexiones.h"

int crearSocket(char *puerto, char *ip, int MaxConexiones)
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
        printf("error en crearSocket\n");
    }

    resultSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if (ip == NULL)
        sockerModoEscucha(&resultSocket, servinfo, MaxConexiones);
    else
        conectarServidor(&resultSocket, servinfo);

    freeaddrinfo(servinfo);

    return resultSocket;
}

void sockerModoEscucha(int *socket, struct addrinfo *servinfo, int MaxConexiones)
{
    bind(*socket, servinfo->ai_addr, servinfo->ai_addrlen);
    listen(*socket, MaxConexiones);
}

void conectarServidor(int *socket, struct addrinfo *servinfo)
{
    int err = 0;
    do
    {
        err = connect(*socket, servinfo->ai_addr, servinfo->ai_addrlen);
    } while (err != -1);
}

int esperarCliente(int *socket)
{
    return accept(*socket, NULL, NULL);
}

void enviarPaqueteResult(int result_cod, int *socket, TipoModulo moduloResponde, TipoModulo moduloRemitente)
{
    // remitente - responde
    t_buffer *buffer = buffer_create(sizeof(int) * 3);
    buffer_add_uint32(buffer, result_cod);
    buffer_add_uint32(buffer, moduloRemitente);
    buffer_add_uint32(buffer, moduloResponde);

    enviarMensaje(socket, buffer, moduloResponde, MENSAJE);
}

int resultadoHandShake(int *socket)
{

    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *op_code = get_opcode_msg_recv(socket);
    TipoModulo remitente;
    TipoModulo responde;
    int respuesta;
    t_buffer *buffer = buffer_leer_recv(socket);
    respuesta = buffer_read_uint32(buffer);
    remitente = buffer_read_uint32(buffer);
    responde = buffer_read_uint32(buffer);
    buffer_destroy(buffer);

    if (respuesta == 1)
        return 1;

    return 0;
}