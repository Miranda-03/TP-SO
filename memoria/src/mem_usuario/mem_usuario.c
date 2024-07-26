#include "mem_usuario.h"

void *espacio;

t_list *tabla_marcos;

t_dictionary *tablas_de_paginas;

int tam_memoria;
int tam_pagina;

int *socket_cpu;

char *path_config_mem_user;

void obtener_socket(int *socket_main)
{
    socket_cpu = socket_main;
}

void iniciar_espacio_usuario(char *path_config)
{
    tam_memoria = atoi(obtenerValorConfig(path_config, "TAM_MEMORIA"));
    espacio = malloc(tam_memoria);
    path_config_mem_user = path_config;
}

void iniciar_marcos()
{
    tam_pagina = atoi(obtenerValorConfig(path_config_mem_user, "TAM_PAGINA"));
    float cant_marcos = (float)tam_memoria / tam_pagina;

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
    t_log *log = log_create("logs/mem_info.log", "Memoria Usuario", 1, LOG_LEVEL_INFO);
    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    dictionary_put(tablas_de_paginas, char_pid, tabla_de_paginas);
    log_info(log, "PID: %d - Crear tabla de páginas de tamaño: %d", PID, 0);

    log_destroy(log);
}

void quitar_tabla_de_pagina(int pid)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", pid);

    t_log *log = log_create("logs/mem_info.log", "Memoria Usuario", 1, LOG_LEVEL_INFO);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    log_info(log, "PID: %d - Quitar tabla de páginas de tamaño: %d", pid, list_size(tabla_de_paginas));
    liberar_memoria(pid);

    dictionary_remove(tablas_de_paginas, char_pid);

    log_destroy(log);
}

void resize_proceso(int PID, int tam, int enviar_dato)
{
    int cant_paginas = cant_paginas_por_proceso(PID);

    int resultado = 1;

    t_log *log = log_create("logs/mem_info.log", "Memoria Usuario", 1, LOG_LEVEL_INFO);

    if (cant_paginas * tam_pagina > tam)
    {
        resultado = quitar_paginas(PID, tam, cant_paginas);
        log_info(log, "PID: %d - Tamaño actual: %d - Tamaño a reducir: %d", PID, cant_paginas * tam_pagina, tam);
    }
    else if (cant_paginas * tam_pagina < tam)
    {
        resultado = agregar_paginas(PID, tam, cant_paginas);
        log_info(log, "PID: %d - Tamaño actual: %d - Tamaño a aumentar: %d", PID, cant_paginas * tam_pagina, tam);
    }

    if (enviar_dato > 0)
        enviar_dato_a_modulo(&resultado, 4, NULL);

    log_destroy(log);
}

void encontrar_marco(int PID, int numero_de_pagina)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", PID);

    t_log *log = log_create("logs/mem_info.log", "Memoria Usuario", 1, LOG_LEVEL_INFO);

    t_list *tabla_de_paginas = (t_list *)dictionary_get(tablas_de_paginas, char_pid);

    bool encontrar_pagina(void *value)
    {
        Pagina *entrada = (Pagina *)value;

        if (entrada->numero_de_pagina == numero_de_pagina)
            return 1;
        return 0;
    }

    Pagina *entrada = (Pagina *)list_find(tabla_de_paginas, encontrar_pagina);

    log_info(log, "PID: %d - Pagina: %d - Marco: %d", PID, numero_de_pagina, entrada->marco->numero_de_marco);

    enviar_dato_a_modulo(&(entrada->marco->numero_de_marco), 4, NULL);

    log_destroy(log);
}

void escribir_memoria(int PID, int direccion_fisica, int bytes_a_escribir, void *dato, int *socket)
{

    t_log *log = log_create("logs/mem_info.log", "Memoria Usuario", 1, LOG_LEVEL_INFO);

    memcpy(espacio + direccion_fisica, dato, bytes_a_escribir);

    int resultado = 1;

    free(dato);

    enviar_dato_a_modulo(&resultado, 4, socket);

    log_info(log, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño: %d ", PID, direccion_fisica, bytes_a_escribir);
    log_destroy(log);
}

void leer_memoria(int PID, int direccion_fisica, int bytes_a_leer, int *socket)
{
    void *dato = malloc(bytes_a_leer);
    t_log *log = log_create("logs/mem_info.log", "Memoria Usuario", 1, LOG_LEVEL_INFO);
    memcpy(dato, espacio + direccion_fisica, bytes_a_leer);

    enviar_dato_a_modulo(dato, bytes_a_leer, socket);

    free(dato);
    log_info(log, "PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño: %d ", PID, direccion_fisica, bytes_a_leer);
    log_destroy(log);
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

        // log_info(log, "PID: %d - Pagina: %d - Marco: %d", PID, num_pagina, num_marco); // revisar si como acceso contamos esto o no, solo una vez en la pagina "base"

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
            entrada->marco->ocupado = 1;
            entrada->numero_de_pagina = cant_paginas;
        }

        list_add(tabla_de_paginas, entrada);
        // log_info(log, "PID: %d - Pagina: %d - Marco: %d", PID, num_pagina, num_marco); // revisar si como acceso contamos esto o no, solo una vez en la pagina "base"

        cant_paginas += 1;
    }

    return 1;
}

int liberar_memoria(int pid)
{
    char char_pid[10];

    snprintf(char_pid, sizeof(char_pid), "%d", pid);

    if (!dictionary_has_key(tablas_de_paginas, char_pid))
        return -1;

    t_list *tabla_de_paginas = dictionary_get(tablas_de_paginas, char_pid);

    void marcar_frames_libres(void *value)
    {
        Pagina *entrada = (Pagina *)value;
        entrada->marco->ocupado = 0;
    }

    list_iterate(tabla_de_paginas, marcar_frames_libres);

    void liberar_paginas(void *value)
    {
        Pagina *entrada = (Pagina *)value;
        free(entrada);
    }

    list_destroy_and_destroy_elements(tabla_de_paginas, liberar_paginas);

    //dictionary_remove(tablas_de_paginas, char_pid);

    return 1;
}

Marco *encontrar_marco_libre()
{
    bool esta_libre(void *value)
    {
        Marco *entrada = (Marco *)value;
        if (entrada->ocupado == 0)
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

    return list_size(tabla_de_paginas);
}

void enviar_tam_de_pagina()
{
    enviar_dato_a_modulo(&tam_pagina, 4, NULL);
}

void enviar_dato_a_modulo(void *dato, int bytes_a_enviar, int *socket)
{
    int *socket_conn = socket_cpu;
    if (socket != NULL)
        socket_conn = socket;

    t_buffer *buffer = buffer_create(bytes_a_enviar);
    buffer_add(buffer, dato, bytes_a_enviar);
    enviarMensaje(socket_conn, buffer, MEMORIA, MENSAJE);
}