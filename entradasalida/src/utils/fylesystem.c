#include "fylesystem.h"

#define BLOCK_COUNT 64 // Número total de bloques se saca de la config

void cargar_bitmap() {
    FILE *file = fopen("bitmap.dat", "rb");
    if (file == NULL) {
        perror("Error al abrir bitmap.dat");
        exit(1);
    }
    fread(bitmap, sizeof(char), CANT_BLOCK, file);
    fclose(file);
}


void guardar_bitmap() {
    FILE *file = fopen("bitmap.dat", "wb");
    if (file == NULL) {
        perror("Error al abrir bitmap.dat");
        exit(EXIT_FAILURE);
    }
    fwrite(bitmap, sizeof(char), CANT_BLOCK, file);
    fclose(file);
}

int buscar_bloque_libre() {
    cargar_bitmap();
    for (int i = 0; i < CANT_BLOCK; i++) {
        if (bitmap[i] == 0) { 
            return i;
        }
    }
    return -1;
}


void marcar_bloque_ocupado(int bloque) {
    cargar_bitmap();
    if (bloque >= 0 && bloque < CANT_BLOCK) {
        bitmap[bloque] = 1; 
        guardar_bitmap();
    }
}

void marcar_bloque_libre(int bloque) {
    cargar_bitmap();
    if (bloque >= 0 && bloque < CANT_BLOCK) {
        bitmap[bloque] = 0; 
        guardar_bitmap();
    }
}
