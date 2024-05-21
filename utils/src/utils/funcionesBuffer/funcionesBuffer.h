#ifndef BUFFER_FUNC_H
#define BUFFER_FUNC_H

#include <utils/structs/structSendRecvMSG.h>

/**
* @fn    buffer_create
* @brief Crea un buffer vacío de tamaño size y offset 0.
*/
t_buffer *buffer_create(uint32_t size);

/**
* @fn    buffer_destroy
* @brief Libera la memoria asociada al buffer.
*/
void buffer_destroy(t_buffer *buffer);

/**
* @fn    buffer_leer_recv
* @brief Arma el buffer que recive del recv().
*/
void buffer_leer_stream_recv(int *socket)

/**
* @fn    buffer_add
* @brief Agrega un stream al buffer en la posición actual y avanza el offset.
*/
void buffer_add(t_buffer *buffer, void *data, uint32_t size);

/**
* @fn    buffer_read
* @brief Guarda size bytes del principio del buffer en la dirección data y avanza el offset.
*/
void buffer_read(t_buffer *buffer, void *data, uint32_t size);

/**
* @fn    buffer_add_uint32
* @brief Agrega un uint32_t al buffer.
*/
void buffer_add_uint32(t_buffer *buffer, uint32_t data);

/**
* @fn    buffer_read_uint32
* @brief Lee un uint32_t del buffer y avanza el offset.
*/
uint32_t buffer_read_uint32(t_buffer *buffer);

/**
* @fn    buffer_add_string
* @brief Agrega string al buffer con un uint32_t adelante indicando su longitud.
*/
void buffer_add_string(t_buffer *buffer, uint32_t length, char *string);

/**
* @fn    buffer_read_string
* @brief Lee un string y su longitud del buffer y avanza el offset.
*/
char *buffer_read_string(t_buffer *buffer, uint32_t *length);

#endif