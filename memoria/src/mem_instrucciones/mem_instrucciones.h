#ifndef MEMORIA_INSTRUCCIONES
#define MEMORIA_INSTRUCCIONES

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/dictionary.h>

/**
* @fn    crear_mem_instrucciones
* @brief crea un diccionario para guardar los PID y las instrucciones de cada proceso.
*/
void crear_mem_instrucciones();

/**
* @fn    agregar_instrucciones
* @brief agrega las instrucciones indicadas en el PATH con su PID al diccionario.
*/
void agregar_instrucciones(char *path, int pid);

/**
* @fn    obtener_instruccion
* @brief retorna una instruccion.
*/
int obtener_instruccion(int pid, int pc);


#endif