#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/structs/structProcesos.h>
#include "Globales/globales.h"



Pcb *crearPcb();
void iniciar_proceso(t_list* list_ready,t_list* list_exec,char* path);
void finalizar_proceso(t_list* listReady,t_list* listExec, t_list* listBlock,int pid);
//void planificarFIFO(t_queue *cola);
//void planificarRR(t_queue *cola);
int asignar_pid();
void llenarbuffer(t_buffer* buffer,Pcb* pcb);
void cambiarGrado(int grado);
void loggearLista(void *elemento);
void reingresar_proceso(MotivoDesalojo* motivo,Pcb* pcb);