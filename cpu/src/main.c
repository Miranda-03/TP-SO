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

char *path_config;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <ruta_archivo_configuracion>\n", argv[0]);
        return EXIT_FAILURE;
    }

    path_config = argv[1];

    procesoCPU = malloc(sizeof(Contexto_proceso));

    // Declaración de los sockets como punteros para poder modificarlos dentro de conectarModuloCPU
    int *CPUSocketMemoriaPtr = &CPUSocketMemoria;
    int *CPUsocketBidireccionalDispatchPtr = &CPUsocketBidireccionalDispatch;
    int *CPUsocketBidireccionalInterruptPtr = &CPUsocketBidireccionalInterrupt;

    conectarModuloCPU(CPUSocketMemoriaPtr, CPUsocketBidireccionalDispatchPtr, CPUsocketBidireccionalInterruptPtr, procesoCPU, &interrupcion, path_config);

    iniciar_TLB(CPUSocketMemoria, path_config);
    iniciar_loger_conn_memoria();

    cicloDeEjecucion(&CPUSocketMemoria, &CPUsocketBidireccionalDispatch, &CPUsocketBidireccionalInterrupt, procesoCPU, &interrupcion);

    return 0;
}
