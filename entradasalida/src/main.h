
#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"


t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, int, t_log*, t_config*);


t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("entradasalida.log","entradasalida",1,LOG_LEVEL_INFO);
	if(nuevo_logger == NULL){
		printf("Error al crear el log");
		exit (1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	nuevo_config = config_create("./entradasalida.config");

	if(nuevo_config==NULL){
		printf("Error con el config");
		exit(2);
	}

	return nuevo_config;
}

void paquete(int conexion)
{
	//Ahora toca lo divertido!

	char* leido;
	t_paquete* paquete;

	paquete = crear_paquete();
    
    leido = readline(">");

	while((strcmp(leido,"")!=0)){
		agregar_a_paquete(paquete,leido,strlen(leido) + 1);
        free(leido);
		leido = readline(">");
    }
	
	enviar_paquete(paquete,conexion);

	eliminar_paquete(paquete);
	
}

void terminar_programa(int conexion1, int conexion2, t_log* logger, t_config* config)
{
	if(logger!=NULL){
		log_destroy(logger);
	}

	if(config!=NULL){
		config_destroy(config);
	}

	liberar_conexion(conexion1);
	liberar_conexion(conexion2);

	//liberamos las conexiones, uso 1 y 2 ya que no importa identificarlas en esta funcion
}


#endif /* CLIENT_H_ */