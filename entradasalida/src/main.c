#include <Conexion/conectar.h>
#include "manage_io/crearIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

int main()
{

    DIR *dir;
    struct dirent *ent;
    int num_files = 0;
    const char *folder_path = "io_config";

    inicializarMutex();

    // Contar el número de archivos en el directorio
    if ((dir = opendir(folder_path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                num_files++;
            }
        }
        closedir(dir);
    }
    else
    {
        perror("No se puede abrir el directorio");
        return EXIT_FAILURE;
    }

    // Crear un array de hilos del tamaño adecuado
    pthread_t *hilos_de_escucha = malloc(num_files * sizeof(pthread_t));
    if (hilos_de_escucha == NULL)
    {
        perror("Error al asignar memoria para los hilos");
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
                char config_path[256];
                snprintf(config_path, sizeof(config_path), "%s/%s", folder_path, ent->d_name);
                crearIO(config_path, ent->d_name, &hilos_de_escucha[thread_index]);
                thread_index++;
            }
        }
        closedir(dir);
    }
    else
    {
        perror("No se puede abrir el directorio");
        free(hilos_de_escucha);
        return EXIT_FAILURE;
    }


    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_files; i++)
    {
        pthread_join(hilos_de_escucha[i], NULL);
    }

    free(hilos_de_escucha);
    return 0;
}
