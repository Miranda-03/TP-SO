#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_
#include <stdlib.h>
#include <stdio.h>

#define PUERTO_KERNEL "4444"
#define PUERTO_MEMORIA "5555"
#define PUERTO_CPU "5555"

/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/
void decir_hola(char* quien);

#endif
