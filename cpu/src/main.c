#include <stdlib.h>
#include <stdio.h>
#include "Conexion/conectar.h"
#include <utils/structs/structSendContextCPU.h>
#include "ciclo/cicloDeEjecucion.h"
#include "TLB/TLB.h"

Contexto_proceso *procesoCPU;
int CPUSocketMemoria;
int CPUsocketBidireccionalDispatch;
int CPUsocketBidireccionalInterrupt;
int interrupcion;

int main(int argc, char *argv[])
{
    procesoCPU = malloc(sizeof(Contexto_proceso));

    // Declaración de los sockets como punteros para poder modificarlos dentro de conectarModuloCPU
    int *CPUSocketMemoriaPtr = &CPUSocketMemoria;
    int *CPUsocketBidireccionalDispatchPtr = &CPUsocketBidireccionalDispatch;
    int *CPUsocketBidireccionalInterruptPtr = &CPUsocketBidireccionalInterrupt;

    conectarModuloCPU(CPUSocketMemoriaPtr, CPUsocketBidireccionalDispatchPtr, CPUsocketBidireccionalInterruptPtr, procesoCPU, &interrupcion);

    iniciar_TLB(CPUSocketMemoria);

    cicloDeEjecucion(&CPUSocketMemoria, &CPUsocketBidireccionalDispatch, &CPUsocketBidireccionalInterrupt, procesoCPU, &interrupcion);

    return 0;
}
