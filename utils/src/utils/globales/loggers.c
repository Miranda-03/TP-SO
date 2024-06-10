#include "loggers.h"

t_log * logger_memoria;

void inicializar_loggers(){
    logger_memoria = log_create();
}