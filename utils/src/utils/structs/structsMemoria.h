#ifndef MEM_STRUCT
#define MEM_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TAMANO_PAGINA 4096  
#define NUMERO_PAGINAS 256 
#define TAMANO_MEMORIA (TAMANO_PAGINA * NUMERO_PAGINAS)  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANO_PAGINA 32 
#define TAMANO_MEMORIA 4096 


typedef struct {
    int numero_pagina;
    int desplazamiento;
} DireccionLogica;

typedef struct {
    int frame;
    int presente; 
} EntradaTablaPaginas;


typedef struct {
    EntradaTablaPaginas* entradas;
    int numero_entradas;
} TablaPaginas;


typedef struct {
    void* espacio_memoria;
    TablaPaginas* tablas_paginas;
    int numero_procesos;
} Memoria;