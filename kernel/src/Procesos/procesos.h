#include <utils/structs/structProcesos.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include "Globales/globales.h"

void planificacionDeProcesos(int caso,char* path);
Pcb *crearPcb(int quantum);
void iniciar_proceso(t_list* list_new);
int proceso_ready(t_list* listNew,t_list* listReady,char* path);
int ejecutar_proceso(t_list* listready,t_list* listexec);
void planificarFIFO(t_queue *cola);
void planificarRR(t_queue *cola);
t_log* iniciar_logger(void);
void enviarproceso(int *KernelSocketCPUDispatch, Pcb* pcb);
int asignar_pid();