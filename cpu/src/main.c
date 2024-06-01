#include <stdlib.h>
#include <stdio.h>
#include "Conexion/conectar.h"
#include <utils/structs/structSendContextCPU.h>
#include "ciclo/cicloDeEjecucion.h"

Contexto_proceso *procesoCPU;
int CPUSocketMemoria;
int CPUsocketBidireccionalDispatch;
int CPUsocketBidireccionalInterrupt;
int interrupcion;

int main(int argc, char *argv[])
{
    procesoCPU = malloc(sizeof(Proceso));

    // Declaración de los sockets como punteros para poder modificarlos dentro de conectarModuloCPU
    int *CPUSocketMemoriaPtr = &CPUSocketMemoria;
    int *CPUsocketBidireccionalDispatchPtr = &CPUsocketBidireccionalDispatch;
    int *CPUsocketBidireccionalInterruptPtr = &CPUsocketBidireccionalInterrupt;

    conectarModuloCPU(CPUSocketMemoriaPtr, CPUsocketBidireccionalDispatchPtr, CPUsocketBidireccionalInterruptPtr, procesoCPU, &interrupcion);

    //cicloDeEjecucion(CPUSocketMemoria, CPUsocketBidireccionalDispatch, CPUsocketBidireccionalInterrupt, procesoCPU, &interrupcion);

    return 0;
}
