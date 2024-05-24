#ifndef CONN_MEMORIA_H
#define CONN_MEMORIA_H

#include <utils/funcionesBuffer/funcionesBuffer.h>


/**
* @fn    recibirInstruccion
* @brief obtiene la instruccion de la memoria.
*/
char* recibirInstruccion(int *socket, unsigned int pid, unsigned int pc);

#endif