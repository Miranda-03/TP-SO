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

    int respuestaHandshake = resultadoHandShake(socket);

    if (respuestaHandshake == 1)
    {
        printf("SE CONECTA CON KERNEL\n");
        
    }
    else
    {
        printf("ERROR CON KERNEL\n");
    }
}
