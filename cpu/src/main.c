#include <stdlib.h>
#include <stdio.h>
#include "Conexion/conectar.h"
#include <utils/structs/structSendContextCPU.h>
#include "ciclo/cicloDeEjecucion.h"

Contexto_proceso *procesoCPU;
int no;
int CPUsocketBidireccionalDispatch;
int CPUsocketBidireccionalInterrupt;
int interrupcion;

int main(int argc, char* argv[]) {

    procesoCPU = malloc(sizeof(Proceso));

    conectarModuloCPU(&no, &CPUsocketBidireccionalDispatch, &CPUsocketBidireccionalInterrupt, procesoCPU, &interrupcion);
    
    //cicloDeEjecucion(&CPUSocketMemoria, &CPUsocketBidireccionalDispatch, &CPUsocketBidireccionalInterrupt, procesoCPU, &interrupcion);

    return 0;
}
