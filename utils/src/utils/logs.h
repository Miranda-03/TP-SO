#include <commons/log.h>

t_log* logger_Kernel = iniciar_logger("kernel");

t_log* iniciar_logger(void,char * modulo)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create(strcat(modulo,".log"),strcat(modulo,"_LOG"),1,LOG_LEVEL_INFO);
	if(nuevo_logger == NULL){
		printf("Error al crear el log");
		exit (1);
	}

	return nuevo_logger;
}