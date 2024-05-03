#ifndef OPCODE_ENUM
#define OPCODE_ENUM

/**
* @brief enum para los 4 tipos de interfaces.
* @param HANDSHAKE para enviar un handshake 
* @param MENSAJE para mandar un mensaje 
* @param PROCESO para mandar un PROCESO 
*/
typedef enum {
    HANDSHAKE,
    MENSAJE,
    PROCESO
} op_code;

#endif