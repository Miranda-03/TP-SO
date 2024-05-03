#ifndef ESTADO_ENUM
#define ESTADO_ENUM

/**
* @brief enum para los 5 estados de ejecucion.
* @param NEW 
* @param READY 
* @param EXEC
* @param EXIT
* @param BLOCK
*/
typedef enum {
    NEW,
    READY,
    EXEC,
    EXIT,
    BLOCK
} ContextoDeEjecucion;

#endif
