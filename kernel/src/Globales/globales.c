#include "Globales/globales.h"

t_config* iniciar_configkernel(void) {
    t_config* nuevo_config;
    nuevo_config = config_create("./kernel.config");

    if (nuevo_config == NULL) {
        printf("Error con el config");
        exit(2);
    }

    return nuevo_config;
}

t_log* iniciar_loggerKernel(void) {
    t_log* nuevo_logger = log_create("kernel.log", "KERNEL_LOG", 1, LOG_LEVEL_INFO);
    if (nuevo_logger == NULL) {
        printf("Error al crear el log");
        exit(1);
    }
    return nuevo_logger;
}

void initialize(void) {
    interfaces_conectadas = dictionary_create();
    logger_kernel = iniciar_loggerKernel();
    config_kernel = iniciar_configkernel();
    listaReady = list_create();
    listaBlock = list_create();
    listaExec = list_create();
    quantum_global = config_get_int_value(config_kernel, "QUANTUM");
    grado_multiprogamacion = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");
    pid_global = 1;
    KernelSocketCPUDispatch = 0;
    KernelSocketCPUInterrumpt = 0;
    KernelSocketMemoria = 0;
    KernelsocketEscucha = 0;
    interfaces_conectadas = dictionary_create();

}