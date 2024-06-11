#include "Kernelconn.h"

void *manage_conn_kernel(void *ptr)
{
    int socketKernel = *((int *)ptr);

    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(&socketKernel);
        op_code *op_code = get_opcode_msg_recv(&socketKernel);
        t_buffer *buffer = buffer_leer_recv(&socketKernel);

        if (buffer_read_uint32(buffer) == 1)
        {
            unsigned int pid = buffer_read_uint32(buffer);
            enviar_mensaje(&socketKernel, agregar_instrucciones(obtener_path_instruccion(buffer),pid));
        }
        else
        {
            // quitar archivo de instrucciones
        }
        buffer_destroy(buffer);
    }
}

char *obtener_path_instruccion(t_buffer *buffer)
{
    int path_len = buffer_read_uint32(buffer);
    return buffer_read_string(buffer, path_len);
}

unsigned int obtener_instuccion_kernel(void *stream)
{
    unsigned int instruccion;
    memcpy(&instruccion, stream, sizeof(unsigned int));
    return instruccion;
}

void enviar_mensaje(int *socket, int instruccion_guardada)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t));
    buffer_add_uint32(buffer, instruccion_guardada);
    enviarMensaje(socket, buffer, MEMORIA, MENSAJE);
}


void crearProceso(Memoria *memoria,  int pid) {
  

    TablaPaginas *tablaPaginasProceso = (TablaPaginas *)malloc(sizeof(TablaPaginas));
    inicializarTablaPaginas(tablaPaginasProceso, NUM_PAGINA);

    // Aquí podrías guardar la tabla de páginas del proceso en una estructura global o devolverla
    // para que el kernel la administre. Por ahora solo imprimimos un mensaje de éxito.
    printf("Proceso %u creado con una tabla de páginas vacía.\n", pid);
}