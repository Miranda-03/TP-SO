#ifndef OBTENER_IP
#define OBTENER_IP

#include <stdio.h> 
#include "commons/config.h"
#include <string.h>


/**
* @fn    obtenerValorConfig
* @brief retorna el valor de la key en el archivo de configuracion del modulo.
*/
char *obtenerValorConfig(char *path, char *key);

#endif