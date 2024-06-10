#ifndef GLOBALES_H
#define GLOBALES_H

#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <Procesos/consola.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include "utils/structs/structProcesos.h"
#include <utils/enums/algorimos.h>



extern t_dictionary *interfaces_conectadas;
extern t_log *logger_kernel;
extern t_config *config_kernel;
extern t_list* listaReady;
extern t_list* listaBlock;
extern t_list* listaExec;
extern t_list* listaQuantum;
extern t_queue * cola_new;
extern int quantum_global;
extern int grado_multiprogamacion;
extern int pid_global;
extern int KernelSocketCPUDispatch;
extern int KernelSocketCPUInterrumpt;
extern int KernelSocketMemoria;
extern int KernelsocketEscucha;
extern int KernelsocketIO;
extern int *KernelsocketBidireccionalIO;
extern int *KernelsocketBidireccionalDispatch;
extern int *KernelsocketBidireccionalInterrupt;
extern Algoritmo algoritmo_actual;

t_config* iniciar_configkernel(void);
t_log* iniciar_loggerKernel(void);
void initialize(void);




#endif // GLOBALES_H