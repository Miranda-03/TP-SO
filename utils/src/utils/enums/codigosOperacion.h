#ifndef OPCODE_ENUM
#define OPCODE_ENUM

/**
* @brief enum para los 4 tipos de interfaces.
* @param HANDSHAKE para enviar un handshake 
* @param MENSAJE para mandar un mensaje 
* 

*/
typedef enum {
    HANDSHAKE,
    MENSAJE,
    PROCESO,
    OBTENER_INSTRUCCION,
    LEER_MEMORIA,
    ESCRIBIR_MEMORIA,
    RESIZE,
    OBTENER_MARCO
} op_code;

#endif