#include "crearConexiones.h"

void solicitar_ip(const char *server_ip, const char *port, t_config *config, char *ip_a_modificar, t_log *loger)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char *mensaje = "SOLICITAR_IP";
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(servaddr);

    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Permitir broadcast en el socket
    int broadcast_enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0)
    {
        log_error(loger, "setsockopt SO_BROADCAST failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0)
    {
        log_error(loger, "Dirección invalida o no soportada");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Enviar solicitud
    if (sendto(sockfd, mensaje, strlen(mensaje), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        log_error(loger, "Falla en el sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    log_info(loger, "Broadcast de solicitud de IP realizado");

    // Recibir respuesta
    int n = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&servaddr, &len);
    if (n < 0)
    {
        log_error(loger, "Falla en el recvfrom");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';

    char *mensaje_loger = string_new();
    string_append(&mensaje_loger, "IP recibida: ");
    string_append(&mensaje_loger, buffer);

    log_info(loger, mensaje_loger);

    config_set_value(config, ip_a_modificar, buffer);
    config_save(config);

    close(sockfd);
}

void escucharYResponder(const char *puerto, t_log *loger)
{
    int sockfd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    char buffer[BUF_SIZE];
    char ip[INET_ADDRSTRLEN];

    sockfd = crearSocketEscucha(puerto);

    char *mensaje_de_escucha = string_new();
    string_append(&mensaje_de_escucha, "Escuchando mensaje broadcast en el puerto: ");
    string_append(&mensaje_de_escucha, puerto);

    log_info(loger, mensaje_de_escucha);

    while (1)
    {
        len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
        {
            log_error(loger, "Falla en el recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';

        char *mensaje_de_solicitud = string_new();
        string_append(&mensaje_de_solicitud, "Solicitud recibida: ");
        string_append(&mensaje_de_solicitud, buffer);

        log_info(loger, mensaje_de_solicitud);

        if (strcmp(buffer, "SOLICITAR_IP") == 0)
        {
            get_local_ip(ip, INET_ADDRSTRLEN);

            char *mensaje_enviar = string_new();
            string_append(&mensaje_enviar, "Enviando IP: ");
            string_append(&mensaje_enviar, ip);

            log_info(loger, mensaje_enviar);

            sendto(sockfd, ip, strlen(ip), 0, (const struct sockaddr *)&cliaddr, len);
            break;
        }
        else
        {
            log_error(loger, "Mensaje de solicitud desconocido");
        }
    }

    close(sockfd);
}

int crearSocketEscucha(const char *puerto)
{
    int sockfd;
    struct sockaddr_in servaddr;

    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(puerto));

    // Asociar el socket con la dirección
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void get_local_ip(char *buffer, size_t buflen)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET)
        {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            buffer, buflen, NULL, 0, NI_NUMERICHOST);
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }

            // if (strcmp(buffer, "127.0.0.1") != 0)
            //     break;
        }
    }

    freeifaddrs(ifaddr);
}