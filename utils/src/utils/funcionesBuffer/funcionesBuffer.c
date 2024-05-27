#include "funcionesBuffer.h"

t_buffer *buffer_create(uint32_t size)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = size;
    buffer->offset = 0;
    buffer->stream = malloc(size);
    return buffer;
}

void buffer_destroy(t_buffer *buffer)
{
    free(buffer->stream);
    free(buffer);
}

t_buffer *buffer_leer_recv(int *socket){
    t_buffer *buffer = buffer_create(0);
    recv(socket, &(buffer->size), sizeof(uint32_t), 0);
    buffer->stream = malloc(buffer->size);
    recv(socket, buffer->stream, buffer->size, 0);
    return buffer;
}

void *buffer_leer_stream_recv(int *socket){
    printf("esto no funciona\n");
}

void buffer_add(t_buffer *buffer, void *data, uint32_t size)
{
    memcpy(buffer->stream + buffer->offset, &data, size);
    buffer->offset += size;
}

void buffer_read(t_buffer *buffer, void *data, uint32_t size)
{
    memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
}

void buffer_add_uint32(t_buffer *buffer, uint32_t data)
{
    buffer_add(buffer, data, sizeof(uint32_t));
}

uint32_t buffer_read_uint32(t_buffer *buffer)
{
    uint32_t data;
    buffer_read(buffer, &data, sizeof(uint32_t));
    return data;
}

void buffer_add_uint8(t_buffer *buffer, uint8_t data)
{
    buffer_add(buffer, data, sizeof(uint8_t));
}

uint32_t buffer_read_uint8(t_buffer *buffer)
{
    uint32_t data;
    buffer_read(buffer, &data, sizeof(uint8_t));
    return data;
}

void buffer_add_string(t_buffer *buffer, uint32_t length, char *string)
{
    buffer_add_uint32(buffer, length);
    buffer_add(buffer, string, length);
}

char *buffer_read_string(t_buffer *buffer, uint32_t *length)
{
    char *string;
    buffer_read(buffer, string, length);
    return string;
}