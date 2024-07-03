#include "mem_usuario.h"

void *espacio;

t_list *tabla_marcos;

t_dictionary *tablas_de_paginas;

int tam_memoria;
int tam_pagina;

int *socket_cpu;
int *socket_stdin;
int *socket_stdout;

void obtener_socket_stdin(int *socket_io)
{
    socket_stdin = socket_io;
}

void obtener_socket_stdout(int *socket_io)
{
    socket_stdout = socket_io;
}

void obtener_socket(int *socket_main)
{
    socket_cpu = socket_main;
}

void iniciar_espacio_usuario()
{
    tam_memoria = atoi(obtenerValorConfig("memoria.config", "TAM_MEMORIA"));
    espacio = malloc(tam_memoria);
}

void iniciar_marcos()
{
    tam_pagina = atoi(obtenerValorConfig("memoria.config", "TAM_PAGINA"));
    int cant_marcos = tam_memoria / tam_pagina;

    tabla_marcos = list_create();

    for (int i = 0; i < cant_marcos; i++)
    {
        Marco *entrada = malloc(sizeof(Marco));
        entrada->numero_de_marco = i;
        entrada->ocupado = 0; // 0 si esta libre y 1 si esta ocupado
        list_add(tabla_marcos, entrada);
    }
}

void iniciar_tablas_de_paginas()
{
    tablas_de_paginas = dictionary_create();
}

void crear_tabla_de_pagina(int PID)
{
    t_list *tabla_de_paginas = list_create();

    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    dictionary_put(tablas_de_paginas, char_pid, tabla_de_paginas);
}

void resize_proceso(int PID, int tam)
{
    int cant_paginas = cant_paginas_por_proceso(PID);

    int resultado = 1; 

    if (cant_paginas * tam_pagina > tam)
    {
        resultado = quitar_paginas(PID, tam, cant_paginas - 1);
    }
    else if (cant_paginas * tam_pagina < tam)
    {
        resultado = agregar_paginas(PID, tam, cant_paginas + 1);
    }

    enviar_dato_a_modulo(&resultado, 4, NULL);
}

void encontrar_marco(int PID, int numero_de_pagina)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    bool encontrar_pagina(void *value)
    {
        Pagina *entrada = (Pagina *)value;

        if (entrada->numero_de_pagina == numero_de_pagina)
            return 1;
        return 0;
    }

    Pagina *entrada = (Pagina *)list_find(tabla_de_paginas, encontrar_pagina);

    enviar_dato_a_modulo(&(entrada->marco->numero_de_marco), 4, NULL);
}

void escribir_memoria(int direccion_fisica, int bytes_a_escribir, void *dato, TipoInterfaz *interfaz)
{
    memcpy(espacio + direccion_fisica, dato, bytes_a_escribir);

    int resultado = 1;
    enviar_dato_a_modulo(&resultado, 4, interfaz);
}

void leer_memoria(int direccion_fisica, int bytes_a_leer, TipoInterfaz *interfaz)
{
    void *dato = malloc(bytes_a_leer);

    memcpy(dato, espacio + direccion_fisica, bytes_a_leer);

    enviar_dato_a_modulo(dato, bytes_a_leer, interfaz);
}

int quitar_paginas(int PID, int tam, int cant_paginas)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    while (cant_paginas * tam_pagina > tam)
    {
        Pagina *entrada = (Pagina *)list_remove(tabla_de_paginas, list_size(tabla_de_paginas) - 1);
        entrada->marco->ocupado = 0;
        free(entrada);
        cant_paginas -= 1;
    }

    return 1;
}

int agregar_paginas(int PID, int tam, int cant_paginas)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    while (cant_paginas * tam_pagina < tam)
    {
        Pagina *entrada = malloc(sizeof(Pagina));
        entrada->marco = NULL;
        entrada->marco = encontrar_marco_libre();

        if (entrada->marco == NULL)
        {
            free(entrada);
            return -1;
        }
        else
        {
            entrada->numero_de_pagina = cant_paginas;
        }

        list_add(tabla_de_paginas, entrada);

        cant_paginas += 1;
    }

    return 1;
}

Marco *encontrar_marco_libre()
{
    bool esta_libre(void *value)
    {
        Marco *entrada = (Marco *)value;
        if(entrada->ocupado == 0)
            return 1;
        return 0;
    }

    return (Marco *)list_find(tabla_marcos, esta_libre);
}

int cant_paginas_por_proceso(int PID)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    return tam_pagina * list_size(tabla_de_paginas);
}

void enviar_tam_de_pagina()
{
    enviar_dato_a_modulo(&tam_pagina, 4, NULL);
}

void enviar_dato_a_modulo(void *dato, int bytes_a_enviar, TipoInterfaz *interfaz)
{
    int *socket_conn = obtener_socket_correspondiente(interfaz);

    //if(*socket_conn < 0) // ERROR

    t_buffer *buffer = buffer_create(bytes_a_enviar);
    buffer_add(buffer, dato, bytes_a_enviar);
    enviarMensaje(socket_conn, buffer, MEMORIA, MENSAJE);
}

int *obtener_socket_correspondiente(TipoInterfaz *interfaz)
{
    if(interfaz == NULL)
        return socket_cpu;
    else if(*interfaz == STDIN)
        return socket_stdin;
    else if(*interfaz == STDOUT)
        return socket_stdout;
}