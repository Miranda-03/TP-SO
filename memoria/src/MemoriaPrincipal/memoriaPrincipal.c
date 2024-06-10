#include "memoriaPrincipal.h"

void inicializar_memoria(Memoria* memoria, int numero_procesos) {
    memoria->espacio_memoria = malloc(TAMANO_MEMORIA);
    memoria->tablas_paginas = (TablaPaginas*) malloc(numero_procesos * sizeof(TablaPaginas));
    memoria->numero_procesos = numero_procesos;

    for (int i = 0; i < numero_procesos; i++) {
        memoria->tablas_paginas[i].entradas = (EntradaTablaPaginas*) malloc((TAMANO_MEMORIA / TAMANO_PAGINA) * sizeof(EntradaTablaPaginas));
        memoria->tablas_paginas[i].numero_entradas = TAMANO_MEMORIA / TAMANO_PAGINA;

        for (int j = 0; j < memoria->tablas_paginas[i].numero_entradas; j++) {
            memoria->tablas_paginas[i].entradas[j].frame = -1;
            memoria->tablas_paginas[i].entradas[j].presente = 0;
        }
    }
}

void cargar_instrucciones(Memoria* memoria, int id_proceso, const char* archivo_pseudocodigo) {
    FILE* archivo = fopen(archivo_pseudocodigo, "r");
    if (archivo == NULL) {
        log_error(logger_memoria, "Error al abrir el archivo\n");
        return;
    }

    int pagina_indice = 0;
    char linea[256];

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (pagina_indice >= memoria->tablas_paginas[id_proceso].numero_entradas) {
            log_error(logger_memoria, "ENo hay suficiente memoria para cargar las instrucciones\n");
            break;
        }

        void* direccion_pagina = memoria->espacio_memoria + pagina_indice * TAMANO_PAGINA;
        memoria->tablas_paginas[id_proceso].entradas[pagina_indice].marco = pagina_indice;
        memoria->tablas_paginas[id_proceso].entradas[pagina_indice].presente = 1;

        strncpy((char*)direccion_pagina, linea, TAMANO_PAGINA);
        pagina_indice++;
    }

    fclose(archivo);
}