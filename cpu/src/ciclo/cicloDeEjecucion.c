#include <cicloDeEjecucion.h>



void cicloDeEjecucion (int *socketMemoria, int *socketDISPATCH, int *socketINTERRUPT, Proceso *procesoCPU){
    while(1){

        if(*procesoCPU != NULL){

            // FETCH
            char* instruccion = recibirInstruccion(socketMemoria, procesoCPU->pid, procesoCPU->registros.pc);

            // DECODE
            char* instruccionSeparada[] = string_split(instruccion, " ");

            execute(instruccionSeparada, procesoCPU);

            checkInterrupt();
        }
    }
}

void execute(char* instruccion[], Proceso *procesoCPU){

    char operacion = instruccion[0];

    switch (operacion)
    {
    case SET:
        /* code */
        break;
    case SUM:
        /* code */
        break;
    case SUB:
        /* code */
        break;
    case JNZ:
        /* code */
        break;
    case IO_GEN_SLEEP:
        /* code */
        break;
    
    default:
        break;
    }
}