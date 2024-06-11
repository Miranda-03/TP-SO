#ifndef MEM_STRUCT
#define MEM_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structProcesos.h"

#define TAM_PAGINA 32 
#define TAM_MEMORIA 4096 
#define NUM_PAGINA 128
#define NUM_FRAMES 128
typedef struct {
    void* datos; 
} Pagina;

typedef struct {
    Pagina* paginas[NUM_PAGINA]; 
} TablaPaginas;

typedef struct {
    void* espacio_usuario; 
    TablaPaginas tabla_paginas; 
} Memoria;

typedef struct {
    Pcb pcb; 
    TablaPaginas tabla_paginas; 
} ProcesoMemoria;


#endif