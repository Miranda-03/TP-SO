#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "entradasalida.config"

#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/enviarMensajes/enviarMensaje.h>

/**
* @fn    conectarModuloIO
* @brief conecta el modulo I/O con los demas componentes.
* @param esGenerico indicar si la interfaz es GENERICA con un 1.
*/
void conectarModuloIO(int *esGenerico);

/**
* @fn    realizarHandshakeIO
* @brief intenta realizar el handshake del módulo IO con la memoria y el kernel
*/
void realizarHandshakeIO(int *socket);

#endif