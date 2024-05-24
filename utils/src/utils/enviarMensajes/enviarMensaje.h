#ifndef MSG_H
#define MSG_H

#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/ModulosEnum.h>
#include <utils/enums/codigosOperacion.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

/**
 * @fn enviarMensaje
* @brief funcion arma parquete con el buffer y luego envia el stream.
*/
void enviarMensaje(int *socket, t_buffer *buffer, TipoModulo modulo, op_code codigoOperacion);

/**
 * @fn get_opcode_msg_recv
* @brief Retorna el op_code del paquete del recv().
*/
op_code *get_opcode_msg_recv(int *socket);

/**
 * @fn get_modulo_msg_recv
* @brief Retorna el modulo del paquete del recv().
*/
TipoModulo *get_modulo_msg_recv(int *socket);

#endif

