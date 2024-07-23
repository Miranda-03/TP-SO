#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/structs/structSendRecvMSG.h>

typedef struct
{
    TipoInterfaz tipoIterfaz;
} HandshakeMessageIO;


void conectarModuloIO(TipoInterfaz tipo_interfaz, char* identificador, int *IOsocketKernel, int *IOsocketMemoria, char* path_config)
{
    t_config *config = config_create(path_config); 

    *IOsocketKernel = crearSocket(config_get_string_value(config, "PUERTO_KERNEL"), config_get_string_value(config, "IP_KERNEL"), 0);
    realizarHandshakeIO(tipo_interfaz, identificador, IOsocketKernel);

    if (tipo_interfaz != GENERICA) 
    {
        *IOsocketMemoria = crearSocket(config_get_string_value(config, "PUERTO_MEMORIA"), config_get_string_value(config, "IP_MEMORIA"), 0);
        realizarHandshakeIO(tipo_interfaz, identificador, IOsocketMemoria);
    }

    config_destroy(config);
}

void realizarHandshakeIO(TipoInterfaz tipo_interfaz, char* identificador, int *socket)
{
    t_buffer *buffer = buffer_create(sizeof(TipoInterfaz) + strlen(identificador) + 1 + sizeof(uint32_t));
    buffer_add_uint32(buffer, tipo_interfaz);
    buffer_add_string(buffer, strlen(identificador) + 1, identificador);
    enviarMensaje(socket, buffer, IO, MENSAJE);

    t_log *loger = log_create("logs/io_conn.log", "conn_b", 1, LOG_LEVEL_INFO);

    int respuestaHandshake = resultadoHandShake(socket);

    if (respuestaHandshake == 1)
    {
        char *mensaje_loger = string_new();
        string_append(&mensaje_loger, "Se crea la conexión, id de interfaz: ");
        string_append(&mensaje_loger, identificador);
        log_info(loger, mensaje_loger);
        free(mensaje_loger);
        
    }
    else
    {
        char *mensaje_loger_error = string_new();
        string_append(&mensaje_loger_error, "Error en la conexión, id de interfaz: ");
        string_append(&mensaje_loger_error, identificador);
        log_error(loger, mensaje_loger_error);
        free(mensaje_loger_error);
    }

    log_destroy(loger);
}

void obtener_ips(char **ip_kernel, char **ip_memoria)
{
    t_config *config = config_create("entradasalida.config");
    t_log *loger = log_create("logs/io_conn.log", "conn_b", 1, LOG_LEVEL_INFO);
    void *ips = solicitar_ip("255.255.255.255", config_get_string_value(config, "PUERTO_KERNEL"), NULL, NULL, loger, "SOLICITAR_IPS");

    char *ips_char = (char *)ips;

    char **ips_separadas = string_split(ips_char, " ");

    string_append(ip_kernel, ips_separadas[0]);
    string_append(ip_memoria, ips_separadas[1]);

    string_array_destroy(ips_separadas);
    free(ips);
    config_destroy(config);
    log_destroy(loger);
}