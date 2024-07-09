#ifndef FYLESYSTEM_H
#define FYLESYSTEM_H

#include "../bitmap/bitmap.h"  // Ruta correcta para incluir bitmap.h
#include "../crearIO.h"

void truncar_archivo(int *pid, t_log *logger, char *nombre_archivo, int nuevo_tamano);
void crear_archivo(int *pid, t_log *logger, char *nombre_archivo);
void eliminar_archivo(int *pid, t_log *logger, char *nombre_archivo);
void leer_archivo_fs(t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo);
void truncar_archivo_fs(int *pid, t_log *logger, char *nombre_archivo, int nuevo_tamano);
void compactar_fs(const char *nombre_archivo);
void mover_bloque(const char *nombre_archivo, int origen, int destino);
void escribir_archivo_fs(t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo);
void leer_archivo(int *pid, t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo);
void escribir_archivo(int *pid, t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo);

#endif /* FYLESYSTEM_H */


