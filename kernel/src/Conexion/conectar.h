#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "kernel.config"
#define MAXCONN 10

/**
* @fn    conectarModuloKernel
* @brief conecta el modulo kernel con los demas componentes.
*/
void conectarModuloKernel();

/**
* @fn    recibirClientes
* @brief función para recibir multiples clientes (multiplexación).
*/
void *recibirClientes(void *ptr);

/**
* @fn    atenderIO
* @brief función para atender al módulo I/O.
*/
void *atenderIO(void *socketComunicacion);

#endif