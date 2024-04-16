#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "entradasalida.config"

/**
* @fn    conectarModuloIO
* @brief conecta el modulo I/O con los demas componentes.
*/
void conectarModuloIO();

/**
* @fn    realizarHandshakeIO
* @brief intenta realizar el handshake del módulo IO con la memoria y el kernel
*/
void realizarHandshakeIO();

#endif