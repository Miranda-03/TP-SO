#ifndef MOTIVOS_DESALOJO_H
#define MOTIVOS_DESALOJO_H

/**
* @brief los motivos del desalojo.
*/
typedef enum {
    FIN_DE_QUANNTUM,
    INTERRUPCION_IO,
    ERROR_DE_PROGRAMA,
    EXIT_SIGNAL,
    INTERRUPCION_KERNEL,
    PETICION_RECURSO
} MotivoDesalojo;

#endif