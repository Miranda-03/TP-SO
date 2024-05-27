#include "globales.h"

t_dictionary *interfaces_conectadas;
t_log *logger_kernel;

void initialize() {
    interfaces_conectadas = dictionary_create();
    logger_kernel = iniciar_loggerKernel();
}

t_log* iniciar_loggerKernel(void) {
    t_log* nuevo_logger = log_create("kernel.log", "KERNEL_LOG", 1, LOG_LEVEL_INFO);
    if (nuevo_logger == NULL) {
        printf("Error al crear el log");
        exit(1);
    }
    return nuevo_logger;
}