#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/structs/structProcesos.h>
#include <utils/enums/motivosDesalojo.h>
#include <utils/enums/instruccionesIO.h>
#include "Globales/globales.h"
#include "CPUConexion/CPUConexion.h"



Pcb *crearPcb();
void iniciar_proceso(char* path);
void finalizar_proceso(int pid,Registros registros);
//void planificarFIFO(t_queue *cola);
//void planificarRR(t_queue *cola);
int asignar_pid();
void llenarbuffer(t_buffer* buffer,Pcb* pcb);
void cambiarGrado(int grado);
void loggearLista(void *elemento);
void manejar_proceso(MotivoDesalojo* motivo,int pid,Registros registros,instruccionIO* instruccion);