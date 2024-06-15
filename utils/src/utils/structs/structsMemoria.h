#ifndef MEM_STRUCT
#define MEM_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structProcesos.h"
#include <commons/collections/dictionary.h>

extern int tam_memoria;
extern int tam_pagina;  
extern int tam_frame; 
extern int cant_paginas;

typedef struct {
    int pid;
    int pagina;
    int presencia;
} Frame;

typedef struct {
    void* espacio_usuario; 
    t_dictionary* tabla_paginas; //Clave = Nro Marco y Valor = pagina
} Memoria;


#endif