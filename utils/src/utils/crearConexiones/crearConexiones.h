#ifndef SOCKET_C
#define SOCKET_C

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <utils/structs/structSendRecvMSG.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>

/**
 * @fn    sockerModoEscucha
 * @brief marcar el socket en modo escucha con listen() y bindiarlo al puerto con bind().
 */
void sockerModoEscucha(int *socket, struct addrinfo *servinfo, int MaxConexiones);

/**
 * @fn    crearSocket
 * @brief configura y retorna un socket.
 * @param puerto puerto del módulo.
 * @param ip IP del módulo a conectar. NULL si el socket es de eschucha.
 * @param MaxConexiones cantidad máxima de conexiones del socket en modo escucha. Dejar en NULL si el socket es de conexion.
 */
int crearSocket(char *puerto, char *ip, int MaxConexiones);

/**
 * @fn esperarCliente
 * @brief espera el módulo cliente y retorna el socket para la comunicación bidireccional
 * @param socket Socket en modo listen del módulo servidor
 */
int esperarCliente(int *socket);

/**
 * @fn conectarServidor
 * @brief intenta conectar con el servidor en un bucle.
 */
void conectarServidor(int *socket, struct addrinfo *servinfo);

/**
 * @fn armarPaqueteResult
 * @brief envia el paquete con la respuesta de handshake.
 */
void enviarPaqueteResult(int result_cod, int *socket, TipoModulo moduloResponde, TipoModulo moduloRemitente);

/**
 * @fn resultadoHandShake
 * @brief recive el resultado del handshake, 1 si fue exitoso y 0 si no.
 */
int resultadoHandShake(int *socket);

#endif