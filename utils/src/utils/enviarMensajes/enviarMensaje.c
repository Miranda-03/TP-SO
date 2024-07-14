#include "enviarMensaje.h"

int enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion)
{
    void *a_enviar;

    if (buffer == NULL)
        a_enviar = malloc(sizeof(TipoModulo) + sizeof(op_code));
    else
        a_enviar = malloc(buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t));

    int offset = 0;
    int bytesEnviados;

    memcpy(a_enviar + offset, &(modulo), sizeof(TipoModulo));
    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(codigoOperacion), sizeof(op_code));

    if (codigoOperacion != HANDSHAKE && codigoOperacion != CHECK_CONN_IO)
    {
        offset += sizeof(op_code);
        memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(a_enviar + offset, buffer->stream, buffer->size);
        bytesEnviados = send(*socket, a_enviar, buffer->size + sizeof(TipoModulo) + sizeof(op_code) + sizeof(uint32_t), 0);
    }
    else
    {
        bytesEnviados = send(*socket, a_enviar, sizeof(TipoModulo) + sizeof(op_code), 0);
    }

    if (buffer != NULL)
        buffer_destroy(buffer);

    free(a_enviar);

    if (bytesEnviados == -1) // manejo de error si no se puede enviar nada
        return -1;

    return bytesEnviados;
}

op_code *get_opcode_msg_recv(int *socket)
{
    op_code *opcode = malloc(sizeof(op_code));
    recv(*socket, opcode, sizeof(op_code), MSG_WAITALL);
    return opcode;
}

TipoModulo *get_modulo_msg_recv(int *socket)
{
    TipoModulo *modulo = malloc(sizeof(TipoModulo));
    recv(*socket, modulo, sizeof(TipoModulo), MSG_WAITALL);
    return modulo;
}

int set_socket_blocking_mode(int sockfd, int blocking)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        return -1;
    }

    if (blocking)
    {
        flags &= ~O_NONBLOCK;
    }
    else
    {
        flags |= O_NONBLOCK;
    }

    if (fcntl(sockfd, F_SETFL, flags) == -1)
    {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

int check_socket_connection(int sockfd)
{
    char buffer;
    // Set the socket to non-blocking mode
    if (set_socket_blocking_mode(sockfd, 0) == -1)
    {
        return 0; // Return 0 indicating an error
    }

    int result = recv(sockfd, &buffer, 1, MSG_PEEK);
    if (result > 0)
    {
        // There is data available to read, the socket is connected
        set_socket_blocking_mode(sockfd, 1); // Set back to blocking mode
        return 1;
    }
    else if (result == 0)
    {
        // The connection has been closed cleanly
        set_socket_blocking_mode(sockfd, 1); // Set back to blocking mode
        return 0;
    }
    else
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            // No data available, but the socket is connected
            set_socket_blocking_mode(sockfd, 1); // Set back to blocking mode
            return 1;
        }
        else
        {
            // Another error, the connection is closed or there's a problem
            set_socket_blocking_mode(sockfd, 1); // Set back to blocking mode
            return 0;
        }
    }
}