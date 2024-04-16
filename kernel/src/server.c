#include "server.h"

/*
int main(void) {

	int  conexion_memoria;
	char* ip;
    char* puerto_memoria;
	char *clave_memoria;

	t_config* config;

	config = iniciar_config();
	ip = config_get_string_value(config,"IP");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	clave_memoria = config_get_string_value(config,"CLAVE_MEMORIA");

	conexion_memoria = crear_conexion(ip, puerto_memoria);

	enviar_mensaje(clave_memoria,conexion_memoria);

	if(logger!=NULL){
		log_destroy(logger);
	}

	if(config!=NULL){
		config_destroy(config);
	}

	liberar_conexion(conexion_memoria); 

	logger = log_create("memoria.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
		
	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
*/
