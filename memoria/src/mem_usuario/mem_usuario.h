#ifndef MEMORIA_USUARIO
#define MEMORIA_USUARIO

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <commons/string.h>
#include <unistd.h>

typedef struct
{
    int numero_de_marco;
    int ocupado;
} Marco;

typedef struct
{
    int numero_de_pagina;
    Marco *marco;
} Pagina;

/**
 * @fn    crear_tabla_de_pagina
 * @brief crea la tabla de paginas vacia del proceso y la agrega al diccionario de tablas de paginas.
 */
void crear_tabla_de_pagina(int PID);

#endif