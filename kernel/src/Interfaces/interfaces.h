#ifndef INTERFACES_H
#define INTERFACES_H

#include <utils/enums/DispositivosIOenum.h>
#include <commons/collections/dictionary.h>

/**
* @fn    guardar_interfaz_conectada
* @brief guarda el socket con el identificador y el tipo de interfaz.
*/
void guardar_interfaz_conectada(int *socket, TipoInterfaz interfaz, char* identificador, t_dictionary *diccionario_interfaces_conectadas);


#endif