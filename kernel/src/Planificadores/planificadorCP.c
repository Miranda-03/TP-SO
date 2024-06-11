#include "planificadorCP.h"

ParamsPCP *params;
int *PIDprocesoEjecutando;
t_temporal *tiempoEjecutando;


/*
    PONER LOS PROTOTIPOS DE LAS FUNCIONES EN EL HEADER 
*/

MensajeProcesoDelCPU *procesoDelCPU

    void *
    planificarCortoPlazo(void *ptr)
{
    params = (ParamsPCP *)ptr;
    PIDprocesoEjecutando = 0;

    while (1)
    {
        if (hayAlgunoEnCPU() < 0)
        {
            if (algoritmo == VRR)
            {
                if (enviarProcesoMayorPrioridadCPU(params->colasCPU->cola_de_quantum, PIDprocesoEjecutando) < 0)
                {
                    *PIDprocesoEjecutando = enviarProcesoCPU(params->colasCP->cola_de_ready);
                } // agreagr cola de quantum
            }
            else
            {
                *PIDprocesoEjecutando = enviarProcesoCPU(params->colasCP->cola_de_ready);
            }

            procesoDelCPU = esperarProcesoCPU();

            if (chequearMotivoIO(procesoDelCPU) < 0 && chequearMotivoExit(procesoDelCPU) < 0 && leQuedaQuantum(procesoDelCPU) < 0)
            {
                agregarProcesoReady(procesoDelCPU);
            }
        }
    }
}

MensajeProcesoDelCPU *esperarProcesoCPU()
{
    // Creo el hilo para escuchar a CPU
    pthread_t hiloEscuchaCPUDispatch;
    pthread_create(&hiloEscuchaCPUDispatch, NULL, escuchaDispatch, (void *)params->KernelSocketCPUDispatch);

    esperarQuantum();
    // Si no devulve nada en ciertos segundos mando interrupcion por fin de quantum
}

int chequearMotivoIO(MensajeProcesoDelCPU *proceso)
{
    if (proceso->motivo != INTERRUPCION_IO)
        return -1;

    enviarProcesoColaCorrespondiente(proceso);
    return 1;
}

void esperarQuantum(){
    tiempoEjecutando = temporal_create();
    
}

void *escuchaDispatch(void *ptr)
{
    
    while (1)
    {
        TipoModulo *modulo = get_modulo_msg_recv(params->socket);
        op_code *op_code = get_opcode_msg_recv(params->socket);
        *PIDprocesoEjecutando = -1;
        
    }
}