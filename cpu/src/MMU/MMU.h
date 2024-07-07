#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <math.h>
#include <commons/string.h>
#include <TLB/TLB.h>

/**
* @fn    instanciar_tam_pagina_MMU
* @brief guardar tamaño de pagina.
*/
void instanciar_tam_pagina_MMU(int tam);

/**
* @fn    traducir_direccion
* @brief devuelve el marco asignado a la pagina.
*/
int traducir_direccion(int num_pagina, int desplazamiento, int pid);

/**
* @fn    obtener_direcciones_fisicas
* @brief devuelve un array con las direcciones fisicas.
*/
char **obtener_direcciones_fisicas(int direccion_logica, int bytes, int pid);

#endif