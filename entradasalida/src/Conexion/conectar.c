#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/structs/structSendRecvMSG.h>

typedef struct
{
    TipoInterfaz tipoIterfaz;
} HandshakeMessageIO;

int IOsocketKernel;
int IOsocketMemoria;

void conectarModuloIO()
{
    IOsocketKernel = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_KERNEL"), obtenerValorConfig(PATH_CONFIG, "IP_KERNEL"), NULL);
    IOsocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);

    realizarHandshakeIO();
}

void realizarHandshakeIO()
{
    printf("Entran los  handshakes IO\n");
    HandshakeMessageIO h_msg = {STDIN};

    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *bufferResponseMemoria = malloc(sizeof(t_buffer));
    t_buffer *bufferResponseKernel = malloc(sizeof(t_buffer));

    buffer->size = 4;
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    memcpy(buffer->stream, &(h_msg.tipoIterfaz), 4);

    t_paquete *paquete = malloc(sizeof(t_paquete));
    t_paquete *paqueteResultMemoria = malloc(sizeof(t_paquete));
    t_paquete *paqueteResultKernel = malloc(sizeof(t_paquete));

    paquete->modulo = IO;
    paquete->buffer = buffer;

    paqueteResultMemoria->buffer = bufferResponseMemoria;
    paqueteResultKernel->buffer = bufferResponseKernel;

    void *a_enviar = malloc(4 + sizeof(uint32_t) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->modulo), sizeof(TipoModulo));


    offset += sizeof(TipoModulo);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    send(IOsocketMemoria, a_enviar, buffer->size + 4 + sizeof(uint32_t), 0); //Manda el handshake en forma de paquete a Memoria
    send(IOsocketKernel, a_enviar, buffer->size + 4 + sizeof(uint32_t), 0); ////Manda el handshake en forma de paquete a Kernel

    recv(IOsocketMemoria, &(paqueteResultMemoria->modulo), 4, 0);
    recv(IOsocketMemoria, &(paqueteResultMemoria->buffer->size), sizeof(uint32_t), MSG_WAITALL); //Recibe el resultado del handshake de memoria

    recv(IOsocketKernel, &(paqueteResultKernel->modulo), 4, 0);
    recv(IOsocketKernel, &(paqueteResultKernel->buffer->size), sizeof(uint32_t), MSG_WAITALL);//Recibe el resultado del handshake de kernel

    void * streamMemoria =  malloc(sizeof(t_resultHandShake));
    paqueteResultMemoria->buffer->stream = streamMemoria;
    recv(IOsocketMemoria, paqueteResultMemoria->buffer->stream, sizeof(t_resultHandShake), 0);

    void * streamKernel =  malloc(sizeof(t_resultHandShake));
    paqueteResultKernel->buffer->stream = streamKernel;
    recv(IOsocketKernel, paqueteResultKernel->buffer->stream, sizeof(t_resultHandShake), 0);


    TipoModulo remitente;
    TipoModulo respondeKernel,respondeMemoria;
    uint8_t respuestaMemoria,respuestaKernel;

    memcpy(&remitente, paqueteResultMemoria->buffer->stream, sizeof(TipoModulo));
    paqueteResultMemoria->buffer->stream += sizeof(TipoModulo);
    memcpy(&remitente, paqueteResultKernel->buffer->stream, sizeof(TipoModulo));
    paqueteResultKernel->buffer->stream += sizeof(TipoModulo);

    memcpy(&respondeMemoria, paqueteResultMemoria->buffer->stream, sizeof(TipoModulo));
    paqueteResultMemoria->buffer->stream += sizeof(TipoModulo);
    memcpy(&respondeKernel, paqueteResultKernel->buffer->stream, sizeof(TipoModulo));
    paqueteResultKernel->buffer->stream += sizeof(TipoModulo);

    memcpy(&respuestaMemoria, paqueteResultMemoria->buffer->stream, sizeof(uint8_t));
    memcpy(&respuestaKernel, paqueteResultKernel->buffer->stream, sizeof(uint8_t));
    

    if (respuestaKernel == 0)
    {
        // Handshake OK
        printf("El handshake con kernel salio bien\n");
    }
    else
    {
        printf("El handshake con kernel salio mal\n");
        // Handshake ERROR
    }

    if (respuestaMemoria == 0)
    {
        // Handshake OK
        printf("El handshake con memoria salio bien\n");
    }
    else
    {
        printf("El handshake con memoria salio mal\n");
        // Handshake ERROR
    }

    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
    free(paqueteResultMemoria);
    free(paqueteResultKernel);
}

