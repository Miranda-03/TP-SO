#include <Conexion/conectar.h>
#include "manage_io/crearIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

char *ip_kernel;
char *ip_memoria;

// Función para comparar el nombre del archivo con los nombres proporcionados, sin la extensión
int is_included(const char *filename, char **include_files, int num_include_files) {
    for (int i = 0; i < num_include_files; i++) {
        if (strncmp(filename, include_files[i], strlen(include_files[i])) == 0 && 
            strcmp(filename + strlen(include_files[i]), ".config") == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *ent;
    int num_files = 0;
    const char *folder_path = "io_config";
    char **include_files = NULL;
    int num_include_files = 0;

    inicializarMutex();

    ip_kernel = string_new();
    ip_memoria = string_new();
    obtener_ips(&ip_kernel, &ip_memoria);

    // Parsear argumentos para obtener los archivos a incluir
    if (argc > 1)
    {
        num_include_files = argc - 1;
        include_files = malloc(num_include_files * sizeof(char *));
        for (int i = 1; i < argc; i++)
        {
            include_files[i - 1] = argv[i];
        }
    }
    else
    {
        printf("No se incluyó ninguna IO en el comando.\n");
        return EXIT_FAILURE;
    }

    // Contar el número de archivos en el directorio que están en la lista de inclusión
    if ((dir = opendir(folder_path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                if (is_included(ent->d_name, include_files, num_include_files))
                {
                    num_files++;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        perror("No se puede abrir el directorio");
        free(include_files);
        return EXIT_FAILURE;
    }

    // Crear un array de hilos del tamaño adecuado
    pthread_t *hilos_de_escucha = malloc(num_files * sizeof(pthread_t));
    if (hilos_de_escucha == NULL)
    {
        perror("Error al asignar memoria para los hilos");
        free(include_files);
        return EXIT_FAILURE;
    }

    int thread_index = 0;

    // Volver a abrir el directorio para leer los archivos y crear los hilos
    if ((dir = opendir(folder_path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                if (is_included(ent->d_name, include_files, num_include_files))
                {
                    char config_path[256];
                    snprintf(config_path, sizeof(config_path), "%s/%s", folder_path, ent->d_name);
                    crearIO(config_path, ent->d_name, &hilos_de_escucha[thread_index], ip_kernel, ip_memoria);
                    thread_index++;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        perror("No se puede abrir el directorio");
        free(hilos_de_escucha);
        free(include_files);
        return EXIT_FAILURE;
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_files; i++)
    {
        pthread_join(hilos_de_escucha[i], NULL);
    }

    free(hilos_de_escucha);
    free(include_files);
    return 0;
}
