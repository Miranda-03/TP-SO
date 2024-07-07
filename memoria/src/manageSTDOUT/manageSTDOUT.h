#ifndef STDOUT_H
#define STDOUT_H

#include <utils/structs/structSendRecvMSG.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <mem_usuario/mem_usuario.h>

void *manage_conn_stdout_io(void *ptr);

#endif