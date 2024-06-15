#include "consola.h"

void consolaInteractiva()
{
    char *leido;

    do
    {
        leido = readline(">");
        int valido = verificar_comando(leido);
        if (valido < 0)
            //log_error(logger_kernel, "Comando no reconocido");
            printf("Comando no reconocido\n");
        else
            atender_instruccion(leido);

    } while (strcmp(leido, "exit") != 0);
}

int verificar_comando(char *leido)
{
    char **comando = string_split(leido, " ");

    if (
        strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ||
        strcmp(comando[0], "INICIAR_PROCESO") == 0 ||
        strcmp(comando[0], "FINALIZAR_PROCESO") == 0 ||
        strcmp(comando[0], "DETENER_PLANIFICACION") == 0 ||
        strcmp(comando[0], "INICIAR_PLANIFICACION") == 0 ||
        strcmp(comando[0], "MULTIPROGRAMACION") == 0 ||
        strcmp(comando[0], "PROCESO_ESTADO"))
    {
        return 1;
    }
    return -1;
}

void atender_instruccion(char *leido)
{
    char **comando = string_split(leido, " ");

    t_buffer *buffer = buffer_create(sizeof(t_buffer));

    /*
         if (strcmp(comando[0], "EJECUTAR_SCRIPT") == 0)
    {
        // Aquí puedes agregar el código para EJECUTAR_SCRIPT
    }
    else
    */

    if (strcmp(comando[0], "INICIAR_PROCESO") == 0)
    {
        char *path = comando[1];
        pthread_t hilo_creacion_proceso;
        pthread_create(&hilo_creacion_proceso, NULL, PLPNuevoProceso, path);
        pthread_join(hilo_creacion_proceso, NULL);
    }
    /*
    else if (strcmp(leido, "FINALIZAR_PROCESO") == 0)
    {
        // Aquí puedes agregar el código para FINALIZAR_PROCESO
    }
    else if (strcmp(leido, "DETENER_PLANIFICACION") == 0)
    {
        // Aquí puedes agregar el código para DETENER_PLANIFICACION
    }
    else if (strcmp(leido, "INICIAR_PLANIFICACION") == 0)
    {
        // Aquí puedes agregar el código para INICIAR_PLANIFICACION
    }
    else if (strcmp(leido, "MULTIPROGRAMACION") == 0)
    {
        // Aquí puedes agregar el código para MULTIPROGRAMACION
    }
    else if (strcmp(leido, "PROCESO_ESTADO") == 0)
    {
        // Aquí puedes agregar el código para PROCESO_ESTADO
    }
    else
    {
        log_error(logger_kernel, "ERROR");
    } */
}
