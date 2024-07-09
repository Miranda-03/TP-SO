#ifndef BITMAP_H
#define BITMAP_H

#include "../fylesystem/fylesystem.h"
#include "utils/obtenerValorConfig/obtenerValorConfig.h"

extern int block_size; 
extern int block_count; 
extern int retraso_compatacion;

extern int bitmap[block_count];

void inicalizar_bloques();
void cargar_bitmap();
void guardar_bitmap();
int buscar_bloque_libre();
void marcar_bloque_ocupado(int bloque);
void marcar_bloque_libre(int bloque);
void crear_archivo_metadata(char *nombre_archivo, int bloque_inicial, int tamano_archivo);
void eliminar_archivo_metadata(char *nombre_archivo);

#endif // BITMAP_H
