#ifndef CONNKERNELCPU_ENUM
#define CONNKERNELCPU_ENUM

/**
* @brief enum para los 2 tipos de conexion del kernel con el CPU.
* @param INTERRUMPT 
* @param DISPATCH 
* @param IO 
*/
typedef enum {
    INTERRUMPT,
    DISPATCH,
    IO
} TipoConn;


#endif