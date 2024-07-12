#ifndef DIALFS_F_IO_H
#define DIALFS_F_IO_H

#include <utils/enums/DispositivosIOenum.h>
#include <pthread.h>
#include <utils/enums/instruccionesIO.h>
#include <commons/config.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <commons/log.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <utils/obtenerValorConfig/obtenerValorConfig.h>
#include <commons/bitarray.h>
#include <math.h>
#include <unistd.h>
#include <utils/enviarMensajes/enviarMensaje.h>
#include <dirent.h>

void iniciar_archivos(char *path_config);

void crear_bitmap(const char *path, const char *filename, size_t size);

void crear_archivo_bloques(const char *path, const char *filename, size_t size);

void crear_archivo(int pid, t_log *loger, char **instruccionSeparada, char *path_base, int cant_bloques);

void borrar_archivo(int pid, t_log *log, char **instruccionSeparada, char *path_base, int cant_bloques);

void truncate_archivo(int pid, t_log *loger, char **instruccionSeparada, char *path_base, int cant_bloques, int size_bloques, int retardo);

int verificar_contiguo(int pid, t_log *loger, t_bitarray *bitmap, int bloque_inicio, int bloques_a_colocar, int bloques_asignados, char *path_base, int size_bloque, int retardo);

int compactar(int pid, t_log *loger, t_bitarray *bitmap, char *path_base, int size_bloque, int retardo);

int encontrar_archivo_y_modificar(int offset, int block_inicio_contador, char *path_base, int size_bloque);

int verificar_espacio_suficiente(t_bitarray *bitmap, int cant_nuevos_bloques);

int escribir_archivo(int pid, t_log *loger, char **instruccionSeparada, int size_bloque, char *path_base, int *socket_memoria);

char *leer_archivo(int pid, t_log *loger, char **instruccionSeparada, int size_bloque, char *path_base, int *socket_memoria);

int obtener_size_del_dato(char **array);

void agregar_bits(t_bitarray *self, int inicio, int tam);

void borrar_bits(t_bitarray *self, int inicio, int tam);

void crear_archivo_metadata(char *path_base, char *nombre_archivo, int bloque_inicial);

int comprobar_espacio(char *path_base, int cant_bloques);

void guardar_bitmap(char *path_base, t_bitarray *self);

t_bitarray *obtener_bitarray_del_archivo(char *path_base, int size);


#endif
