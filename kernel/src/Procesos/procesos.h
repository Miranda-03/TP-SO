#include <utils/structs/structProcesos.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>


void planificacionDeProcesos(int caso,char* path,int pid);
Pcb *crearPcb();
void iniciar_proceso(t_list* list_ready,t_list* list_exec,char* path);
//void planificarFIFO(t_queue *cola);
//void planificarRR(t_queue *cola);
int asignar_pid();
void llenarbuffer(t_buffer* buffer,Pcb* pcb);