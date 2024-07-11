#include "connMemoria.h"

t_log *loger_conn_memoria;

void iniciar_loger_conn_memoria()
{
    loger_conn_memoria = log_create("logs/cpu_conn_memoria.log", "conn_memoria", 1, LOG_LEVEL_INFO);
}

char *recibirInstruccion(int *socket, unsigned int pid, unsigned int pc)
{

    t_buffer *buffer = buffer_create(sizeof(unsigned int) * 2);

    buffer_add_uint32(buffer, pid);
    buffer_add_uint32(buffer, pc);

    enviarMensaje(socket, buffer, CPU, OBTENER_INSTRUCCION);

    TipoModulo *modulo = get_modulo_msg_recv(socket);
    op_code *op_code = get_opcode_msg_recv(socket);
    t_buffer *buffer_recv = buffer_leer_recv(socket);

    int size = buffer_read_uint32(buffer_recv);
    char *instruccion = buffer_read_string(buffer_recv, size);

    buffer_destroy(buffer_recv);

    return instruccion;
}

void *cpu_leer_memoria(int direccion_logica_inicio, int bytes_a_leer, int pid, int socket_memoria)
{
    void *dato = malloc(bytes_a_leer);

    char **direcciones = obtener_direcciones_fisicas(direccion_logica_inicio, bytes_a_leer, pid);

    int size_array_direcciones = string_array_size(direcciones);

    int offset = 0;

    for (int i = 0; i < (size_array_direcciones); i += 2)
    {
        t_buffer *buffer = buffer_create(12);
        buffer_add_uint32(buffer, pid);
        buffer_add_uint32(buffer, atoi(direcciones[i]));
        buffer_add_uint32(buffer, atoi(direcciones[i + 1]));

        enviarMensaje(&socket_memoria, buffer, CPU, LEER_MEMORIA);

        TipoModulo *modulo = get_modulo_msg_recv(&socket_memoria);
        op_code *opcode = get_opcode_msg_recv(&socket_memoria);
        t_buffer *buffer_recv = buffer_leer_recv(&socket_memoria);

        buffer_read(buffer_recv, dato + offset, atoi(direcciones[i + 1]));

        offset += atoi(direcciones[i + 1]);

        buffer_destroy(buffer_recv);
    }

    int numero = *(int)dato;
    char *dato_char = (char *)dato;

    mensaje_conn_memoria(pid, "LEER", direcciones[0], numero);

    return dato;
}

int escribir_memoria(int direccion_logica_inicio, int bytes_a_escribir, int pid, void *dato, int socket_memoria)
{
    char **direcciones = obtener_direcciones_fisicas(direccion_logica_inicio, bytes_a_escribir, pid);

    // int *dato = (int *)dato_v;

    int size_array_direcciones = string_array_size(direcciones);

    int resultado = 0;

    int offset = 0;

    for (int i = 0; i < (size_array_direcciones); i += 2)
    {
        t_buffer *buffer = buffer_create(12 + bytes_a_escribir);
        buffer_add_uint32(buffer, pid);
        buffer_add_uint32(buffer, atoi(direcciones[i]));
        buffer_add_uint32(buffer, atoi(direcciones[i + 1]));
        buffer_add(buffer, dato + offset, atoi(direcciones[i + 1]));

        enviarMensaje(&socket_memoria, buffer, CPU, ESCRIBIR_MEMORIA);

        offset += atoi(direcciones[i + 1]);

        TipoModulo *modulo = get_modulo_msg_recv(&socket_memoria);
        op_code *opcode = get_opcode_msg_recv(&socket_memoria);
        t_buffer *buffer_recv = buffer_leer_recv(&socket_memoria);

        buffer_read(buffer_recv, &resultado, 4);

        buffer_destroy(buffer_recv);

        if (resultado < 0)
            break;
    }

    mensaje_conn_memoria(pid, "ESCRIBIR", direcciones[0], *(int *)dato);

    return resultado;
}

void mensaje_conn_memoria(int pid, char *accion, char *direccion_fisica, int dato)
{
    char *mensaje = string_new();

    // int dato_int = (int) *dato;

    string_append(&mensaje, "PID: ");
    string_append(&mensaje, string_itoa(pid));
    string_append(&mensaje, " - Acción: ");
    string_append(&mensaje, accion);
    string_append(&mensaje, " - Dirección Física: ");
    string_append(&mensaje, direccion_fisica);
    string_append(&mensaje, " - Valor: ");
    string_append(&mensaje, string_itoa(dato));

    log_info(loger_conn_memoria, mensaje);
}