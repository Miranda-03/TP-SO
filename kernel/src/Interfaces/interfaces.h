#ifndef INTERFACES_H
#define INTERFACES_H


#include <Conexion/conectar.h>
#include <pthread.h>


/*
  void *valor = malloc(4 + sizeof(uint32_t));
    memcpy(valor, &interfaz, 4);
    memcpy(valor + 4, &socket, sizeof(uint32_t));
*/
typedef struct
{
    TipoInterfaz interfaz;
    int socket;
} IOguardar;

/**
* @fn    inicializarMutex
* @brief inicializa el mutex para el diccionario de las interfaces conectadas.
*/
void inicializarMutexDiccionarioIOConectadas();

/**
* @fn    guardar_interfaz_conectada
* @brief guarda el socket con el identificador y el tipo de interfaz.
*/
void guardar_interfaz_conectada(int *socket, TipoInterfaz interfaz, char* identificador, t_dictionary *diccionario_interfaces_conectadas);

/**
* @fn    buscarNuevasConectadas
* @brief itera sobre el diccionario y pone los nuevos elementos en una lista.
*/
void buscarNuevasConectadas(t_dictionary *diccionario_interfaces_conectadas,  void(*closure)(char*,void*));

/**
* @fn    devolverKeys
* @brief devuelve un lista con todas las keys del diccionario pasado.
*/
t_list *devolverKeys(t_dictionary *diccionario_interfaces_conectadas);

#endif