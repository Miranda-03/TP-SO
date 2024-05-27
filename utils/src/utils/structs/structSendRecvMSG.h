#ifndef MSG_STRUCT
#define MSG_STRUCT

#include <utils/enums/ModulosEnum.h>
#include <utils/enums/codigosOperacion.h>
#include <stdint.h>

/**
 * @brief buffer .
 * @param size Tamaño del payload
 * @param offset Desplazamiento dentro del payload
 * @param stream Payload
 */
typedef struct
{
    uint32_t size;
    uint32_t offset;
    void *stream;
} t_buffer;

/**
 * @brief buffer .
 * @param modulo cual es el modulo que envia el mensaje
 * @param opcode codigo de operacion
 * @param buffer buffer con el mensaje
 */
typedef struct
{
    TipoModulo modulo;
    op_code opcode;
    t_buffer *buffer;
} t_paquete;

/**
 * @brief struct para enviar como respuesta del handshake.
 * @param moduloRemitente modulo el cual esta pidiendo el handshake.
 * @param moduloResponde modulo que da la respuesta.
 * @param respuesta_cod codigo de respuesta.
 */
typedef struct
{
    TipoModulo moduloRemitente;
    TipoModulo moduloResponde;
    uint32_t respuesta_cod;
} t_resultHandShake;

#endif