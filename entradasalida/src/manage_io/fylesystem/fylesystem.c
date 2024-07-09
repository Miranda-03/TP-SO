#include "fylesystem.h"

void crear_archivo(int *pid, t_log *logger, char *nombre_archivo) {
    int bloque_libre = buscar_bloque_libre();
    if (bloque_libre == -1) {
        bloque_libre = buscar_bloque_libre();
    }

    if (bloque_libre != -1) {
        marcar_bloque_ocupado(bloque_libre);
        crear_archivo_metadata(nombre_archivo, bloque_libre, 0);
        log_info(logger, mensaje_info_detallado(*pid, "Crear Archivo:", nombre_archivo, 0, 0));
    } else {
        log_error(logger, "No hay bloques libres disponibles para crear el archivo.");
    }
}

void eliminar_archivo(int *pid, t_log *logger, char *nombre_archivo) {
    int bloque_inicial = obtener_bloque_inicial(nombre_archivo);
    int tamano = obtener_tamano_archivo(nombre_archivo);
    
    liberar_bloques(bloque_inicial, tamano);
    eliminar_archivo_metadata(nombre_archivo);
    log_info(logger, mensaje_info_detallado(*pid, "Eliminar Archivo:", nombre_archivo, 0, 0));
}

void truncar_archivo(int *pid, t_log *logger, char *nombre_archivo, int nuevo_tamano) {
    int bloque_inicial = obtener_bloque_inicial(nombre_archivo);
    int tamano_actual = obtener_tamano_archivo(nombre_archivo);

    if (nuevo_tamano == tamano_actual) {
        log_info(logger, mensaje_info_detallado(*pid, "Truncar Archivo:", nombre_archivo, nuevo_tamano, 0));
        return;
    }

    truncar_archivo_fs(pid, logger, nombre_archivo, nuevo_tamano);
    log_info(logger, mensaje_info_detallado(*pid, "Truncar Archivo:", nombre_archivo, nuevo_tamano, 0));
}

void truncar_archivo_fs(int *pid, t_log *logger, char *nombre_archivo, int nuevo_tamano) {
    int bloque_inicial;
    int tamano_archivo;

    FILE *metadata_file = fopen(nombre_archivo, "r+");
    if (metadata_file == NULL) {
        log_error(logger, "Error al abrir el archivo de metadata para lectura y escritura");
        return;
    }
    fscanf(metadata_file, "BLOQUE_INICIAL=%d\n", &bloque_inicial);
    fscanf(metadata_file, "TAMANIO_ARCHIVO=%d\n", &tamano_archivo);
    fclose(metadata_file);

    if (nuevo_tamano == tamano_archivo) {
        return;
    }

    int bloques_necesarios_antes = (tamano_archivo + block_size - 1) / block_size;
    int bloques_necesarios_despues = (nuevo_tamano + block_size - 1) / block_size;

    if (nuevo_tamano > tamano_archivo) {
        int bloques_libres_contiguos = 0;
        for (int i = bloque_inicial + bloques_necesarios_antes; i < block_size; i++) {
            if (bitmap[i] == 0) {
                bloques_libres_contiguos++;
                if (bloques_libres_contiguos >= (bloques_necesarios_despues - bloques_necesarios_antes)) {
                    break;
                }
            } else {
                bloques_libres_contiguos = 0;
            }
        }

        if (bloques_libres_contiguos < (bloques_necesarios_despues - bloques_necesarios_antes)) {
            compactar_fs(nombre_archivo);
            sleep(RETRASO_COMPACTACION);

            bloques_libres_contiguos = 0;
            for (int i = bloque_inicial + bloques_necesarios_antes; i < CANT_BLOCK; i++) {
                if (bitmap[i] == 0) {
                    bloques_libres_contiguos++;
                    if (bloques_libres_contiguos >= (bloques_necesarios_despues - bloques_necesarios_antes)) {
                        break;
                    }
                } else {
                    bloques_libres_contiguos = 0;
                }
            }

            if (bloques_libres_contiguos < (bloques_necesarios_despues - bloques_necesarios_antes)) {
                log_error(logger, "No hay suficiente espacio contiguo disponible para truncar el archivo.");
                return;
            }
        }
    }

    for (int i = bloques_necesarios_despues; i < bloques_necesarios_antes; i++) {
        marcar_bloque_libre(bloque_inicial + i);
    }

    metadata_file = fopen(nombre_archivo, "r+");
    if (metadata_file == NULL) {
        log_error(logger, "Error al abrir el archivo de metadata para escritura");
        return;
    }
    fprintf(metadata_file, "BLOQUE_INICIAL=%d\n", bloque_inicial);
    fprintf(metadata_file, "TAMANIO_ARCHIVO=%d\n", nuevo_tamano);
    fclose(metadata_file);
}

void escribir_archivo(int *pid, t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo) {
    escribir_archivo_fs(logger, nombre_archivo, tamano, puntero_archivo);
    log_info(logger, mensaje_info_detallado(*pid, "Escribir Archivo:", nombre_archivo, tamano, puntero_archivo));
}

void escribir_archivo_fs(t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo) {
    int bloque_inicial = obtener_bloque_inicial(nombre_archivo);
    int tamano_actual = obtener_tamano_archivo(nombre_archivo);
    int bloque_inicio = puntero_archivo / block_size;
    int bloque_fin = (puntero_archivo + tamano - 1) / block_size;

    if (bloque_fin >= (tamano_actual + block_size - 1) / block_size) {
        int bloques_necesarios = bloque_fin + 1 - (tamano_actual + block_size - 1) / block_size;
        int bloque_libre = buscar_bloque_libre();

        if (bloque_libre == -1) {
            log_error(logger, "No hay suficientes bloques libres para escribir el archivo.");
            return;
        }

        for (int i = 0; i < bloques_necesarios; i++) {
            marcar_bloque_ocupado(bloque_inicial + bloque_inicio + i);
        }
        tamano_actual += bloques_necesarios * block_size;
        actualizar_tamano_archivo(nombre_archivo, tamano_actual);
    }

    FILE *archivo = fopen(nombre_archivo, "rb+");
    if (archivo == NULL) {
        log_error(logger, "Error al abrir el archivo para escribir datos.");
        return;
    }

    fseek(archivo, (bloque_inicial + bloque_inicio) * block_size + puntero_archivo % block_size, SEEK_SET);
    fwrite(datos_a_escribir, sizeof(char), tamano, archivo);

    fclose(archivo);

    log_info(logger, "Se han escrito datos en el archivo correctamente.");
}

void leer_archivo(int *pid, t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo) {
    leer_archivo_fs(logger, nombre_archivo, tamano, puntero_archivo);
    log_info(logger, mensaje_info_detallado(*pid, "Leer Archivo:", nombre_archivo, tamano, puntero_archivo));
}

void leer_archivo_fs(t_log *logger, char *nombre_archivo, int tamano, int puntero_archivo) {
    int bloque_inicial = obtener_bloque_inicial(nombre_archivo);
    int tamano_actual = obtener_tamano_archivo(nombre_archivo);

    if (puntero_archivo + tamano > tamano_actual) {
        log_error(logger, "Error: el tamaño de lectura excede el tamaño del archivo.");
        return;
    }

    int bloque_inicio = puntero_archivo / block_size;
    int bloque_fin = (puntero_archivo + tamano - 1) / block_size;

    FILE *archivo = fopen(nombre_archivo, "rb");
    if (archivo == NULL) {
        log_error(logger, "Error al abrir el archivo para leer datos.");
        return;
    }

    fseek(archivo, (bloque_inicial + bloque_inicio) * block_size + puntero_archivo % block_size, SEEK_SET);
    char datos_leidos[tamano];
    fread(datos_leidos, sizeof(char), tamano, archivo);

    fclose(archivo);

    log_info(logger, "Se han leído datos del archivo correctamente.");
}

void compactar_fs(const char *nombre_archivo) {
    cargar_bitmap();

    int contador_escritura = 0;

    for (int contador_lectura = 0; contador_lectura < block_count; contador_lectura++) {
        if (bitmap[contador_lectura] == 1) {
            if (contador_lectura != contador_escritura) {
                mover_bloque(nombre_archivo, contador_lectura, contador_escritura);
                bitmap[contador_escritura] = 1;
                bitmap[contador_lectura] = 0;
            }
            contador_escritura++;
        }
    }

    guardar_bitmap();
}

void mover_bloque(const char *nombre_archivo, int origen, int destino) {
    char buffer[block_size];

    FILE *archivo = fopen(nombre_archivo, "rb+");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para mover bloque");
        exit(1);
    }

    fseek(archivo, origen * block_size, SEEK_SET);
    fread(buffer, sizeof(char), block_size, archivo);

    fseek(archivo, destino * block_size, SEEK_SET);
    fwrite(buffer, sizeof(char), block_size, archivo);

    fclose(archivo);
}
