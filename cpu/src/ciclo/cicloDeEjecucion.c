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
    char* primerParametro = instruccion[1];
    char* segundoParametro = instruccion[2];
    
    switch (operacion)
    {
    case SET:
        void* registro = obtenerRegistro(primerParametro, procesoCPU);
        registro = atoi(&segundoParametro);
        break;
    case SUM:
        
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
    case EXIT:

        break;
    default:
        break;
    }
}

void* obtenerRegistro(char* registro, Proceso *procesoCPU){
    switch (registro)
    {
    case ax:
        return procesoCPU->registros.ax;
        break;
    case eax:
        return procesoCPU->registros.eax;
        break;
    case bx:
        return procesoCPU->registros.bx;
        break;
    case ebx:
        return procesoCPU->registros.ebx;
        break;
    case cx:
        return procesoCPU->registros.cx;
        break;
    case ecx:
        return procesoCPU->registros.ecx;
        break;
    case dx:
        return procesoCPU->registros.dx;
        break;
    case edx:
        return procesoCPU->registros.edx;
        break;
    default:
        break;
    }
}