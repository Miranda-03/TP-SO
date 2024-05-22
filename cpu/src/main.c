#include <stdlib.h>
#include <stdio.h>
#include "Conexion/conectar.h"
#include <utils/structs/structProcesos.h>
#include "ciclo/cicloDeEjecucion.h"

Proceso *procesoCPU;
int CPUSocketMemoria;
int CPUsocketBidireccionalDispatch;
int CPUsocketBidireccionalInterrupt;

int main(int argc, char* argv[]) {

    procesoCPU = malloc(sizeof(Proceso));

    conectarModuloCPU();
    
    cicloDeEjecucion(CPUSocketMemoria, CPUsocketBidireccionalDispatch, CPUsocketBidireccionalInterrupt);

    return 0;
}
