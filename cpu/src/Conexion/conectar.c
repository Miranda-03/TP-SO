#include <Conexion/conectar.h>
#include <utils/crearConexiones/crearConexiones.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>

void conectarModuloCPU(){


    int CPUsocketEscuchaDispatch = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante 
    int CPUsocketBidireccionalDispatch = esperarCliente(CPUsocketEscuchaDispatch); 

    int CPUsocketEscuchaInterrupt = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), NULL, MAXCONN);
    // la siguiente linea es autobloqueante 
    int CPUsocketBidireccionalInterrupt = esperarCliente(CPUsocketEscuchaInterrupt);

    //Conexion con el módulo memoria
    int CPUSocketMemoria = crearSocket(obtenerValorConfig(PATH_CONFIG, "PUERTO_MEMORIA"), obtenerValorConfig(PATH_CONFIG, "IP_MEMORIA"), NULL);
}