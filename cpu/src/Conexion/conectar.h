#ifndef CONECTAR_F
#define CONECTAR_F


#define PATH_CONFIG "cpu.config"
#define MAXCONN 1

/**
* @fn    conectarModuloCPU
* @brief conecta el modulo CPU con los demas componentes.
*/
void conectarModuloCPU();

/**
* @fn    handshakeCPUMemoria
* @brief hacer handshake CPU memoria.
*/
void handshakeCPUMemoria();


#endif