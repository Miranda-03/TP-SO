
#include "funciones_DIALFS.h"

void iniciar_archivos(char *path_config)
{
    t_config *config = config_create(path_config);

    int tam_bloque = config_get_int_value(config, "BLOCK_SIZE");
    int cant_bloques = config_get_int_value(config, "BLOCK_COUNT");

    char *path_base = config_get_string_value(config, "PATH_BASE_DIALFS");

    crear_archivo_bloques(path_base, "bloques", tam_bloque * cant_bloques);

    crear_bitmap(path_base, "bitmap", cant_bloques);

    config_destroy(config);
}

void crear_bitmap(const char *path, const char *filename, size_t size)
{
    // char *bitarray = string_new();
    // t_bitarray *bitarray_struct = bitarray_create_with_mode(bitarray, size, MSB_FIRST);

    // Crear el path completo del archivo
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s.dat", path, filename);

    FILE *file = fopen(fullPath, "a"); // Abrir el archivo en modo binario para escritura
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Asigna memoria para el bitarray
    char *bitarray_data = (char *)malloc(size);
    if (bitarray_data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE); // Salir en caso de error de memoria
    }

    memset(bitarray_data, 0, size);

    // Crea la estructura del bitarray
    t_bitarray *bitarray_struct = bitarray_create_with_mode(bitarray_data, size, MSB_FIRST);
    if (bitarray_struct == NULL)
    {
        perror("bitarray_create_with_mode");
        free(bitarray_data);
        exit(EXIT_FAILURE); // Salir en caso de error
    }

    // Leer el bitarray del archivo
    size_t read_size = fread(bitarray_struct->bitarray, 1, size, file);
    if (read_size == 0)
    {
        size_t write_size = fwrite(bitarray_struct->bitarray, 1, bitarray_struct->size, file);
        if (write_size != bitarray_struct->size)
        {
            perror("fwrite");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    // ftruncate(file, size);

    // fwrite(bitarray_struct->bitarray, bitarray_struct->size, 1, file);

    fclose(file);
}

void crear_archivo_bloques(const char *path, const char *filename, size_t size)
{
    // Crear el path completo del archivo
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s.dat", path, filename);

    FILE *file = fopen(fullPath, "a"); // Abrir el archivo en modo binario para escritura
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    fclose(file); // Cerrar el archivo
}

void crear_archivo(int pid, t_log *loger, char **instruccionSeparada, char *path_base, int cant_bloques)
{
    int bloque_inicial = comprobar_espacio(path_base, cant_bloques);

    if (bloque_inicial < 0)
        return;

    mensaje_info_detallado(pid, instruccionSeparada[0], instruccionSeparada[2], 0, 0, loger);

    crear_archivo_metadata(path_base, instruccionSeparada[2], bloque_inicial);
}

void borrar_archivo(int pid, t_log *log, char **instruccionSeparada, char *path_base, int cant_bloques)
{
    t_bitarray *bitmap = obtener_bitarray_del_archivo(path_base, cant_bloques);

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path_base, instruccionSeparada[2]);

    t_config *metadata = config_create(fullPath);

    int inicio = config_get_int_value(metadata, "BLOQUE_INICIAL");
    int tam = config_get_int_value(metadata, "TAMANIO_ARCHIVO");

    remove(fullPath);

    borrar_bits(bitmap, inicio, tam);

    guardar_bitmap(path_base, bitmap);

    config_destroy(metadata);

    mensaje_info_detallado(pid, instruccionSeparada[0], instruccionSeparada[2], 0, 0, log);
}

void truncate_archivo(int pid, t_log *loger, char **instruccionSeparada, char *path_base, int cant_bloques, int size_bloques, int retardo)
{
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path_base, instruccionSeparada[2]);

    t_config *metadata = config_create(fullPath);

    int inicio = config_get_int_value(metadata, "BLOQUE_INICIAL");
    int tam = config_get_int_value(metadata, "TAMANIO_ARCHIVO");

    int nuevo_tam = atoi(instruccionSeparada[3]);

    t_bitarray *bitmap = obtener_bitarray_del_archivo(path_base, cant_bloques);

    int bloques_asignados = ceil((float)tam / size_bloques);
    if (bloques_asignados == 0)
    {
        bloques_asignados = 1;
    }
    int bloques_nuevos = ceil((float)nuevo_tam / size_bloques);

    int nuevo_inicio = inicio;

    config_destroy(metadata);

    if (bloques_nuevos < bloques_asignados)
    {
        borrar_bits(bitmap, inicio + bloques_nuevos, bloques_asignados - bloques_nuevos);

        t_config *metadata_guardar = config_create(fullPath);
        config_set_value(metadata_guardar, "BLOQUE_INICIAL", string_itoa(nuevo_inicio));
        config_set_value(metadata_guardar, "TAMANIO_ARCHIVO", string_itoa(nuevo_tam));
        config_save(metadata_guardar);
        config_destroy(metadata_guardar);
    }
    else if (bloques_nuevos > bloques_asignados)
    {
        // if (verificar_espacio_suficiente(bitmap, bloques_nuevos - bloques_asignados) > 0)

        borrar_bits(bitmap, inicio, bloques_asignados);
        nuevo_inicio = verificar_contiguo(pid, loger, bitmap, inicio + bloques_asignados, bloques_nuevos - bloques_asignados, bloques_asignados, path_base, size_bloques, retardo);
        agregar_bits(bitmap, nuevo_inicio, bloques_nuevos);

        t_config *metadata_guardar = config_create(fullPath);
        config_set_value(metadata_guardar, "BLOQUE_INICIAL", string_itoa(nuevo_inicio));
        config_set_value(metadata_guardar, "TAMANIO_ARCHIVO", string_itoa(nuevo_tam));
        config_save(metadata_guardar);
        config_destroy(metadata_guardar);
    }

    mensaje_info_detallado(pid, instruccionSeparada[0], instruccionSeparada[2], nuevo_tam, 0, loger);

    guardar_bitmap(path_base, bitmap);
}

int verificar_contiguo(int pid, t_log *loger, t_bitarray *bitmap, int bloque_inicio, int bloques_a_colocar, int bloques_asignados, char *path_base, int size_bloque, int retardo)
{
    int contador = bloque_inicio + 1;
    int bloques_disponibles = 0;

    while (contador < bitmap->size)
    {
        if (!bitarray_test_bit(bitmap, contador))
        {
            bloques_disponibles++;
        }
        else
        {
            break;
        }
        contador++;
    }

    if (bloques_disponibles < bloques_a_colocar)
    {
        return compactar(pid, loger, bitmap, path_base, size_bloque, retardo);
    }

    return bloque_inicio - bloques_asignados;
}

int compactar(int pid, t_log *loger, t_bitarray *bitmap, char *path_base, int size_bloque, int retardo)
{
    log_info(loger, "PID: %d - Inicio Compactación", pid);

    int offset_bitmap = 0;
    int contador = 0;
    int num_bloques = 0;

    while (contador < bitmap->size)
    {
        if (bitarray_test_bit(bitmap, contador))
        {
            num_bloques = encontrar_archivo_y_modificar(offset_bitmap, contador, path_base, size_bloque);
            if (num_bloques != -1)
            {
                borrar_bits(bitmap, contador, num_bloques);
                agregar_bits(bitmap, offset_bitmap, num_bloques);
                mover_datos_guardados(contador, offset_bitmap, num_bloques, size_bloque, path_base);
                offset_bitmap += num_bloques;
            }
        }
        contador++;
    }

    usleep(retardo * 1000);

    log_info(loger, "PID: %d - Fin Compactación", pid);
    return offset_bitmap;
}

void mover_datos_guardados(int contador, int offset, int num_bloques, int size_bloque, char *path_base)
{
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/bloques.dat", path_base);

    FILE *file = fopen(fullPath, "rb+");
    if (!file)
    {
        // log_error(loger, "Error al abrir archivo");
    }

    char *buffer = (char *)malloc(num_bloques * size_bloque);

    fseek(file, (contador * size_bloque), SEEK_SET);
    fread(buffer, (num_bloques * size_bloque), 1, file);

    fseek(file, (offset * size_bloque), SEEK_SET);
    fwrite(buffer, (num_bloques * size_bloque), 1, file);

    free(buffer);
    fclose(file);
}

int encontrar_archivo_y_modificar(int offset, int block_inicio_contador, char *path_base, int size_bloque)
{
    int tam_archivo = 0;
    int tam_archivo_copy = -1;

    struct dirent *entry;
    DIR *dir = opendir(path_base);
    char fullPath[1024];

    if (dir == NULL)
    {
        perror("Error al abrir el directorio");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { // Asegurarse de que es un archivo regular
            const char *filename = entry->d_name;
            size_t len = strlen(filename);
            if (len > 4 && strcmp(filename + len - 4, ".txt") == 0)
            {
                snprintf(fullPath, sizeof(fullPath), "%s/%s", path_base, filename);
                t_config *metadata = config_create(fullPath);
                if (config_get_int_value(metadata, "BLOQUE_INICIAL") == block_inicio_contador)
                {
                    tam_archivo = config_get_int_value(metadata, "TAMANIO_ARCHIVO");
                    tam_archivo_copy = tam_archivo;
                    config_set_value(metadata, "BLOQUE_INICIAL", string_itoa(offset));
                    config_save(metadata);
                    config_destroy(metadata);
                    break;
                }
                config_destroy(metadata);
            }
        }
    }

    closedir(dir);

    if (tam_archivo_copy == 0)
        return 1;

    if (tam_archivo_copy == -1)
        return -1;

    return ceil((float)tam_archivo_copy / size_bloque);
}

int verificar_espacio_suficiente(t_bitarray *bitmap, int cant_nuevos_bloques)
{
    int contador = 0;
    int bloques_disponibles = 0;

    while (contador < bitmap->size)
    {
        if (!bitarray_test_bit(bitmap, (off_t)contador))
        {
            bloques_disponibles++;
        }
        contador++;
    }

    if (bloques_disponibles >= cant_nuevos_bloques)
        return 1;

    return -1;
}

int escribir_archivo(int pid, t_log *loger, char **instruccionSeparada, int size_bloque, char *path_base, int *socket_memoria)
{
    char **direcciones_fisicas = string_get_string_as_array(instruccionSeparada[3]);

    int size_array_dir = string_array_size(direcciones_fisicas);

    char *dato_a_leer = calloc(1, 1); // Inicializa como cadena vacía

    for (int i = 0; i < (size_array_dir); i += 2)
    {
        t_buffer *buffer = buffer_create(12);
        buffer_add_uint32(buffer, pid);
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i]));
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i + 1]));

        enviarMensaje(socket_memoria, buffer, IO, LEER_MEMORIA);

        TipoModulo *modulo = get_modulo_msg_recv(socket_memoria);
        op_code *opcode = get_opcode_msg_recv(socket_memoria);
        t_buffer *buffer_recv = buffer_leer_recv(socket_memoria);

        char *data = buffer_read_string(buffer_recv, atoi(direcciones_fisicas[i + 1]));
        dato_a_leer = realloc(dato_a_leer, strlen(dato_a_leer) + strlen(data) + 1);
        strcat(dato_a_leer, data);

        free(data);
        buffer_destroy(buffer_recv);
        buffer_destroy(buffer);
    }

    int bloque_inicio;

    char fullPathMetadata[1024];
    snprintf(fullPathMetadata, sizeof(fullPathMetadata), "%s/%s", path_base, instruccionSeparada[2]);

    t_config *metadata = config_create(fullPathMetadata);
    bloque_inicio = config_get_int_value(metadata, "BLOQUE_INICIAL");

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/bloques.dat", path_base);

    int offset_archivo = (bloque_inicio * size_bloque) + atoi(instruccionSeparada[4]);

    FILE *file = fopen(fullPath, "rb+");
    if (!file)
    {
        log_error(loger, "Error al abrir archivo");
        free(dato_a_leer);
        config_destroy(metadata);
        return -1;
    }

    fseek(file, offset_archivo, SEEK_SET);

    fwrite(dato_a_leer, 1, strlen(dato_a_leer), file);

    int size_dato_a_leer = strlen(dato_a_leer);
    char size_dato_a_leer_str[20];
    sprintf(size_dato_a_leer_str, "%d", size_dato_a_leer);

    fclose(file);

    mensaje_info_detallado(pid, instruccionSeparada[0], instruccionSeparada[2], size_dato_a_leer, atoi(instruccionSeparada[4]), loger);

    string_array_destroy(direcciones_fisicas);

    free(dato_a_leer);
    config_destroy(metadata);

    return 1;
}

// Función para leer del archivo
char *leer_archivo(int pid, t_log *loger, char **instruccionSeparada, int size_bloque, char *path_base, int *socket_memoria)
{
    char **direcciones_fisicas = string_get_string_as_array(instruccionSeparada[3]);

    int size_dato_a_leer = obtener_size_del_dato(direcciones_fisicas);

    int size_array_dir = string_array_size(direcciones_fisicas);

    char *dato = malloc(size_dato_a_leer + 1);
    if (!dato)
    {
        perror("malloc");
        string_array_destroy(direcciones_fisicas);
        return NULL;
    }
    memset(dato, 0, size_dato_a_leer + 1);

    int bloque_inicio;

    char fullPathMetadata[1024];
    snprintf(fullPathMetadata, sizeof(fullPathMetadata), "%s/%s", path_base, instruccionSeparada[2]);

    t_config *metadata = config_create(fullPathMetadata);
    bloque_inicio = config_get_int_value(metadata, "BLOQUE_INICIAL");

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/bloques.dat", path_base);

    FILE *file = fopen(fullPath, "rb");
    if (!file)
    {
        perror("fopen");
        free(dato);
        string_array_destroy(direcciones_fisicas);
        config_destroy(metadata);
        return NULL;
    }

    int offset_archivo = atoi(instruccionSeparada[4]);

    if (fseek(file, (bloque_inicio * size_bloque) + offset_archivo, SEEK_SET) != 0)
    {
        perror("fseek");
        fclose(file);
        free(dato);
        string_array_destroy(direcciones_fisicas);
        config_destroy(metadata);
        return NULL;
    }

    fread(dato, 1, size_dato_a_leer, file);

    fclose(file);

    int resultado;

    int offset = 0;

    for (int i = 0; i < (size_array_dir); i += 2)
    {
        t_buffer *buffer = buffer_create(4 + 4 + 4 + atoi(direcciones_fisicas[i + 1]));
        buffer_add_uint32(buffer, pid);
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i]));
        buffer_add_uint32(buffer, atoi(direcciones_fisicas[i + 1]));
        buffer_add(buffer, dato + offset, atoi(direcciones_fisicas[i + 1]));

        offset += atoi(direcciones_fisicas[i + 1]);

        enviarMensaje(socket_memoria, buffer, IO, ESCRIBIR_MEMORIA);

        TipoModulo *modulo = get_modulo_msg_recv(socket_memoria);
        op_code *opcode = get_opcode_msg_recv(socket_memoria);
        t_buffer *buffer_recv = buffer_leer_recv(socket_memoria);

        resultado = buffer_read_uint32(buffer_recv);

        if (resultado < 0)
        {
            buffer_destroy(buffer_recv);
            buffer_destroy(buffer);
            break;
        }

        buffer_destroy(buffer_recv);
        buffer_destroy(buffer);
    }

    mensaje_info_detallado(pid, instruccionSeparada[0], instruccionSeparada[2], size_dato_a_leer, offset_archivo, loger);

    string_array_destroy(direcciones_fisicas);
    config_destroy(metadata);

    return dato;
}

int obtener_size_del_dato(char **array)
{
    int size_array = string_array_size(array);

    int size = 0;

    for (int i = 0; i < size_array; i += 2)
    {
        size += atoi(array[i + 1]);
    }

    return size;
}

void agregar_bits(t_bitarray *self, int inicio, int tam)
{
    int tam_nuevo = inicio + tam;
    while (inicio < tam_nuevo)
    {
        bitarray_set_bit(self, inicio);
        inicio++;
    }
}

void borrar_bits(t_bitarray *self, int inicio, int tam)
{
    int tam_nuevo = inicio + tam;
    while (inicio < tam_nuevo)
    {
        bitarray_clean_bit(self, inicio);
        inicio++;
    }
}

void crear_archivo_metadata(char *path_base, char *nombre_archivo, int bloque_inicial)
{
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path_base, nombre_archivo);

    FILE *file = fopen(fullPath, "wb"); // Abrir el archivo en modo binario para escritura
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    t_config *metadata = config_create(fullPath);
    config_set_value(metadata, "BLOQUE_INICIAL", string_itoa(bloque_inicial));
    config_set_value(metadata, "TAMANIO_ARCHIVO", "0");
    config_save(metadata);
    config_destroy(metadata);
}

int comprobar_espacio(char *path_base, int cant_bloques)
{
    t_bitarray *bitmap_struct = obtener_bitarray_del_archivo(path_base, cant_bloques);

    int contador = 0;

    int respuesta = -1;

    while (contador < bitmap_struct->size)
    {
        if (!bitarray_test_bit(bitmap_struct, contador))
        {
            bitarray_set_bit(bitmap_struct, contador);
            respuesta = contador;
            break;
        }

        contador++;
    }

    guardar_bitmap(path_base, bitmap_struct);

    return respuesta;
}

t_bitarray *obtener_bitarray_del_archivo(char *path_base, int size)
{
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/bitmap.dat", path_base);

    // Asigna memoria para el bitarray
    char *bitarray_data = (char *)malloc(size);
    if (bitarray_data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE); // Salir en caso de error de memoria
    }

    // Crea la estructura del bitarray
    t_bitarray *bitarray_struct = bitarray_create_with_mode(bitarray_data, size, MSB_FIRST);
    if (bitarray_struct == NULL)
    {
        perror("bitarray_create_with_mode");
        free(bitarray_data);
        exit(EXIT_FAILURE); // Salir en caso de error
    }

    // Abrir el archivo en modo lectura binaria
    FILE *archivo_bitmap = fopen(fullPath, "rb");
    if (archivo_bitmap == NULL)
    {
        perror("fopen");
        free(bitarray_data); // Liberar la memoria en caso de error
        free(bitarray_struct);
        exit(EXIT_FAILURE);
    }

    // Leer el bitarray del archivo
    size_t read_size = fread(bitarray_struct->bitarray, 1, size, archivo_bitmap);
    if (read_size != size)
    {
        perror("fread");
        free(bitarray_data); // Liberar la memoria en caso de error
        free(bitarray_struct);
        fclose(archivo_bitmap);
        exit(EXIT_FAILURE);
    }

    fclose(archivo_bitmap);

    return bitarray_struct;
}

void guardar_bitmap(char *path_base, t_bitarray *self)
{
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/bitmap.dat", path_base);

    // Abrir el archivo en modo escritura binaria
    FILE *file = fopen(fullPath, "wb");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Escribir el bitarray en el archivo
    size_t write_size = fwrite(self->bitarray, 1, self->size, file);
    if (write_size != self->size)
    {
        perror("fwrite");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void mensaje_info_detallado(int PID, char *operacion, char *nombre_archivo, int tamano, int puntero_archivo, t_log *logger)
{
    if (strcmp(operacion, "IO_FS_CREATE") == 0)
    {
        log_info(logger, "PID: %d - Crear Archivo: %s", PID, nombre_archivo);
    }
    else if (strcmp(operacion, "IO_FS_DELETE") == 0)
    {
        log_info(logger, "PID: %d - Eliminar Archivo: %s", PID, nombre_archivo);
    }
    else if (strcmp(operacion, "IO_FS_TRUNCATE") == 0)
    {
        log_info(logger, "PID: %d - Truncar Archivo: %s - Tamaño: %d", PID, nombre_archivo, tamano);
    }
    else if (strcmp(operacion, "IO_FS_READ") == 0)
    {
        log_info(logger, "PID: %d - Leer Archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %d", PID, nombre_archivo, tamano, puntero_archivo);
    }
    else if (strcmp(operacion, "IO_FS_WRITE") == 0)
    {
        log_info(logger, "PID: %d - Escribir Archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %d", PID, nombre_archivo, tamano, puntero_archivo);
    }
}
