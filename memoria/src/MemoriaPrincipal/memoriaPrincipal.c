#include "memoriaPrincipal.h"

Memoria* inicializar_memoria() {
    
    
    Memoria* mem = (Memoria*)malloc(sizeof(Memoria));

    mem->tabla_paginas = dictionary_create();

    mem->espacio_usuario = malloc(TAM_MEMORIA);
    if (mem->espacio_usuario == NULL) {
        log_error(logger_memoria,"No hay espacio para hacer el malloc de la memoria");
        exit(1);
    }

    return mem;
}


void liberar_memoria(Memoria* mem) {
    if (mem != NULL) {
        for (int i = 0; i < NUM_PAGINA; i++) {
            free(mem->tabla_paginas.paginas[i]);
        }
        free(mem->espacio_usuario);
        free(mem);
    }

    dictionary_clean_and_destroy_elements(mem->tabla_paginas);
}