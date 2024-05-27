#ifndef GLOBALES_H
#define GLOBALES_H

#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <Procesos/procesos.h>
#include <Procesos/consola.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>

extern t_dictionary *interfaces_conectadas;
extern t_log *logger_kernel;

t_log* iniciar_loggerKernel(void);
void initialize();

#endif // GLOBALES_H
