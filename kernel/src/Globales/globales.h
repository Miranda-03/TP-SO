#ifndef GLOBALES_H
#define GLOBALES_H

#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <Procesos/procesos.h>
#include <Procesos/consola.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>


extern t_dictionary *interfaces_conectadas;
extern t_log *logger_kernel;
extern t_config *config_kernel;
extern int quantum_global;
extern int grado_multiprogamacion;
extern int pid_global = 1;

t_log* iniciar_loggerKernel(void);
t_config* iniciar_configkernel();
void initialize();


#endif // GLOBALES_H