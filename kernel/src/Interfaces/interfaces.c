#include "interfaces.h"

pthread_mutex_t mutexDiccionarioInterfaz;

void inicializarMutexDiccionarioIOConectadas()
{
    pthread_mutex_init(&mutexDiccionarioInterfaz, NULL);
}

void guardar_interfaz_conectada(int *socket, TipoInterfaz interfaz, char *identificador, t_dictionary *diccionario_interfaces_conectadas)
{

    IOguardar *io = (IOguardar *)malloc(sizeof(IOguardar));
    io->interfaz = interfaz;
    io->socket = socket;

    pthread_mutex_lock(&mutexDiccionarioInterfaz);
    dictionary_put(diccionario_interfaces_conectadas, identificador, io);
    pthread_mutex_unlock(&mutexDiccionarioInterfaz);
}

void buscarNuevasConectadas(t_dictionary *diccionario_interfaces_conectadas, void (*closure)(char *, void *))
{
    pthread_mutex_lock(&mutexDiccionarioInterfaz);
    dictionary_iterator(diccionario_interfaces_conectadas, closure);
    pthread_mutex_unlock(&mutexDiccionarioInterfaz);
}

t_list *devolverKeys(t_dictionary *diccionario_interfaces_conectadas)
{
    t_list *keys_ids = list_create();
    pthread_mutex_lock(&mutexDiccionarioInterfaz);
    keys_ids = dictionary_keys(diccionario_interfaces_conectadas);
    pthread_mutex_unlock(&mutexDiccionarioInterfaz);

    return keys_ids;
}