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

void realizarHandshakeIO(int *s1,int *s2) //No funciona el handshake
{
    printf("Entran los  handshakes IO\n");

    HandshakeMessageIO h_msg = {STDIN};

    void *stream = malloc(sizeof(HandshakeMessageIO));

    memcpy(stream, &(h_msg.tipoIterfaz), sizeof(TipoInterfaz));

    send(IOsocketKernel, stream, sizeof(HandshakeMessageIO), 0);
    send(IOsocketMemoria, stream, sizeof(HandshakeMessageIO), 0);

    int respuestaKernel = resultadoHandShake(IOsocketKernel);
    int respuestaMemoria = resultadoHandShake(IOsocketMemoria);

    
    
    free(stream);

    if (respuestaKernel == 1)
    {
        // Handshake OK
        printf("El handshake con kernel salio bien\n");
    }
    else
    {
        printf("El handshake con kernel salio mal\n");
        // Handshake ERROR
    }

    if (respuestaMemoria == 1)
    {
        // Handshake OK
        printf("El handshake con memoria salio bien\n");
    }
    else
    {
        printf("El handshake con memoria salio mal\n");
        // Handshake ERROR
    }

    
}

