#include "main.h"

int main(int argc, char* argv[]) {
    
    int conexion_memoria;
	char* ip;
    char* puerto_memoria;
	char *clave_memoria;

	t_log* logger;
	t_config* config;

    logger = iniciar_logger();

    config = iniciar_config();

    //obtenemos las direcciones de puertos e ip's en config
    ip = config_get_string_value(config,"IP");
    puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    clave_memoria = config_get_string_value(config,"CLAVE_MEMORIA");

    //creo las conexiones
   
	conexion_memoria = crear_conexion(ip, puerto_memoria);


	// Enviamos a los servidores
    enviar_mensaje(clave_memoria,conexion_memoria);


	//Armamos y enviamos los paquetes

    paquete(conexion_memoria);

	terminar_programa(conexion_memoria, logger, config);

    return 0;
}

