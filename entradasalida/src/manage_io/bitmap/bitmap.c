#include "bitmap.h"

void inicalizar_bloques(){
    int valor = obtenerValorConfig("../entradasalida.config", "BLOCK_SIZE");
    block_size = atoi(valor); 
    valor = obtenerValorConfig("../entradasalida.config", "BLOCK_COUNT");
    block_count = atoi(valor);
    valor = obtenerValorConfig("../entradasalida.config", "RETRASO_COMPACTACION");
    retraso_compatacion = atoi(valor);
    free(valor); 

}

void cargar_bitmap() {
    FILE *file = fopen("bitmap.dat", "rb");
    if (file == NULL) {
        perror("Error al abrir bitmap.dat");
        exit(1);
    }
    fread(bitmap, sizeof(char), block_count, file);
    fclose(file);
}

void guardar_bitmap() {
    FILE *file = fopen("bitmap.dat", "wb");
    if (file == NULL) {
        perror("Error al abrir bitmap.dat");
        exit(1);
    }
    fwrite(bitmap, sizeof(char), block_count, file);
    fclose(file);
}

int buscar_bloque_libre() {
    cargar_bitmap();
    for (int i = 0; i < block_count; i++) {
        if (bitmap[i] == 0) {
            return i;
        }
    }
    return -1;
}

void marcar_bloque_ocupado(int bloque) {
    cargar_bitmap();
    if (bloque >= 0 && bloque < block_count) {
        bitmap[bloque] = 1;
        guardar_bitmap();
    }
}

void marcar_bloque_libre(int bloque) {
    cargar_bitmap();
    if (bloque >= 0 && bloque < block_count) {
        bitmap[bloque] = 0;
        guardar_bitmap();
    }
}

void crear_archivo_metadata(char *nombre_archivo, int bloque_inicial, int tamano_archivo) {
    FILE *file = fopen(nombre_archivo, "w");
    if (file == NULL) {
        perror("Error al crear el archivo de metadata");
        exit(1);
    }

    fprintf(file, "BLOQUE_INICIAL=%d\n", bloque_inicial);
    fprintf(file, "TAMANIO_ARCHIVO=%d\n", tamano_archivo);

    fclose(file);
}

void eliminar_archivo_metadata(char *nombre_archivo) {
    if (remove(nombre_archivo) != 0) {
        perror("Error al eliminar el archivo de metadata");
    }
}
