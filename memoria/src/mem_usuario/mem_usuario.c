#include "mem_usuario.h"

void *espacio;

t_list *tabla_marcos;

t_dictionary *tablas_de_paginas;

int tam_memoria;
int tam_pagina;

void iniciar_espacio_usuario() // HEADER
{
    tam_memoria = atoi(obtenerValorConfig("memoria.config", "TAM_MEMORIA"));
    espacio = malloc(tam_memoria);
}

void iniciar_marcos() // HEADER
{
    tam_pagina = atoi(obtenerValorConfig("memori.config", "TAM_PAGINA"));
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

void iniciar_tablas_de_paginas() // HEADER
{
    tablas_de_paginas = dictionary_create();
}

void crear_tabla_de_pagina(int PID) // HEADER
{
    t_list *tabla_de_paginas;
    = list_create();

    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    dictionary_put(tablas_de_paginas, char_pid, tabla_de_paginas);
}

int resize_proceso(int PID, int tam) // HEADER
{
    int cant_paginas = cant_paginas_por_proceso(PID);

    if (cant_paginas * tam_pagina > tam)
    {
        quitar_paginas(PID, tam, cant_paginas - 1);
    }
    else if (cant_paginas * tam_pagina < tam)
    {
        agregar_paginas(PID, tam, cant_paginas + 1);
    }
}

int encontrar_marco(int PID, int numero_de_pagina) // HEADER
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

    return entrada->marco->numero_de_marco;
}

int escribir_memoria(int numero_de_marco, int desplazamiento, int bytes, void *dato) // HEADER
{
    // int puntero = (numero_de_marco * tam_pagina) + desplazamiento;

    int puntero = obtener_puntero_a_memoria(numero_de_marco, desplazamiento); // FALTA HACER

    if (puntero < 0)
        return -1; // DIRECCION INVALIDA

    int cant_bytes_copiados = 1;

    while (cant_bytes_copiados <= bytes)
    {
        int puntero_donde_escribir = excedio_pagina(puntero + cant_bytes_copiados); // FALTA HACER

        if (puntero_donde_escribir < 0)
            return -2; // NO PUEDE GUARDAR EL DATO POR FALTA DE MEMORIA

        memcpy(espacio + puntero_donde_escribir + (cant_bytes_copiados - 1), dato + (cant_bytes_copiados - 1), 1);

        cant_bytes_copiados += 1;
    }

    return 1;
}

void *leer_memoria(int numero_de_marco, int desplazamiento, int bytes_a_leer) // HEADER
{
    int puntero = obtener_puntero_a_memoria(numero_de_marco, desplazamiento); // FALTA HACER

    if (puntero < 0)
        return -1; // DIRECCION INVALIDA

    if (valor_fuera_de_rango(puntero, bytes_a_leer) < 0)
        return -3; // SE QUIERE LEER UNA PARTE DE LA MEMORIA QUE NO EXISTE

    void *dato = malloc(bytes_a_leer);

    int bytes_leidos = 1;

    while (bytes_leidos <= bytes_a_leer)
    {
        int puntero_donde_leer = excedio_pagina(puntero + bytes_leidos); // FALTA HACER

        memcpy(dato + (bytes_leidos - 1), espacio + puntero_donde_leer + (bytes_leidos - 1), 1);

        bytes_leidos += 1;
    }

    return 1;
}

void quitar_paginas(int PID, int tam, int cant_paginas) // HEADER
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
}

void agregar_paginas(int PID, int tam, int cant_paginas) // HEADER
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    while (cant_paginas * tam_pagina < tam)
    {
        Pagina *entrada = malloc(sizeof(Pagina));
        entrada->marco = encontrar_marco_libre();

        if (entrada->marco == NULL)
        {
            // ENVIA MENSAJE DE OUT OF MEMORY A CPU
            free(entrada);
        }
        else
        {
            entrada->numero_de_pagina = cant_paginas;
        }

        list_add(tabla_de_paginas, entrada);

        cant_paginas += 1;
    }
}

Marco *encontrar_marco_libre() // HEADER
{
    bool esta_libre(void *value)
    {
        Marco *marco = (Marco *)value;
        if (marco->ocupado == 0)
            return 1;
        return 0;
    }

    Marco *entrada = (Marco *)list_find(tabla_marcos, esta_libre);

    if (entrada == NULL)
        return NULL;

    entrada->ocupado = 1;

    return entrada;
}

int cant_paginas_por_proceso(int PID) // HEADER
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    return tam_pagina * list_size(tabla_de_paginas);
}