#ifndef MEM_STRUCT
#define MEM_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structProcesos.h"
#include <commons/collections/dictionary.h>

#define TAM_PAGINA 32 
#define TAM_MEMORIA 4096  /*tiene que estar en el config, esto se cambia*/
#define NUM_PAGINA 128
#define NUM_FRAMES 128

typedef struct {
    void* datos; 
    int presencia;
} Pagina;

typedef struct {
    void* datos;
    int presencia;
} Frame;

typedef struct {
    void* espacio_usuario; 
    t_dictionary* tabla_paginas; //Clave = Nro Marco y Valor = pagina
} Memoria;

typedef struct {
    //datos de proceso
} ProcesoMemoria;


#endif