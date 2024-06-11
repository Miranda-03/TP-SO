#include "mem_instrucciones.h"

t_dictionary *memoria_instrucciones;

void crear_mem_instrucciones()
{
   memoria_instrucciones = dictionary_create();
}

int agregar_instrucciones(char* path, int pid) {
    FILE* file_instrucciones = fopen(path, "r");
    if (file_instrucciones == NULL)
        return -1;

    char char_pid[10];
    snprintf(char_pid, 10, "%d", pid);
    dictionary_put(memoria_instrucciones, char_pid, file_instrucciones);

    return 1;
}


char* obtener_instruccion( int pid,  int pc) {
    char char_pid[10];
    snprintf(char_pid, 10, "%d", pid);
    FILE* file = dictionary_get(memoria_instrucciones, char_pid);

    if (file == NULL)
        return "null";

    unsigned int contador = 0;
    char* instruccion = NULL;
    size_t len = 0;
    while ((getline(&instruccion, &len, file)) != -1) {
        if (contador == pc)
            return instruccion;
        contador++;
    }

    return "null";
}

void cargar_pagina(Memoria* memoria, int pid, const char* archivo_pseudocodigo, int pagina_indice) {
    
    FILE* archivo = fopen(archivo_pseudocodigo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    int inicio_pagina = pagina_indice * TAM_PAGINA;
    int pc = inicio_pagina;
    int indice_memoria;

    while (!feof(archivo) && pc < inicio_pagina + TAMA_PAGINA) {
        char* instruccion = obtener_instruccion(pid, pc);
        if (strcmp(instruccion, "null") == 0) {
            break;
        }
        indice_memoria = pagina_indice * TAM_PAGINA + (pc - inicio_pagina);
        memcpy(memoria->espacio_memoria + indice_memoria, instruccion, strlen(instruccion));
        pc += strlen(instruccion) + 1; 

        free(instruccion);
    }

    fclose(archivo);
}
