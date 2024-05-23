#ifndef CICLO_H
#define CICLO_H

#include <connMemoria/connMemoria.h>
#include <commons/string.h>
#include <utils/enums/instrucciones.h>
#include <utils/enums/instruccionesIO.h>
#include <utils/structs/structProcesos.h>

/**
* @fn    cicloDeEjecucion
* @brief El ciclo de ejecución del CPU.
*/
void cicloDeEjecucion(CPUSocketMemoria, CPUsocketBidireccionalDispatch, CPUsocketBidireccionalInterrupt, Proceso *procesoCPU);

/**
* @fn    execute
* @brief Fase de execute del CPU.
*/
void execute(char* instruccion[], Proceso *procesoCPU);

#endif