#include "interfaces.h"

void guardar_interfaz_conectada(int *socket, TipoInterfaz interfaz, char* identificador, t_dictionary *diccionario_interfaces_conectadas){
    void* valor = malloc(4 + sizeof(uint32_t));
    memcpy(valor, &interfaz, 4);
    memcpy(valor + 4, socket, sizeof(uint32_t));
    
    dictionary_put(diccionario_interfaces_conectadas, identificador, valor);
}