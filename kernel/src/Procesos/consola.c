#include "consola.h"

void consolaInteractiva()
{
    char *linea;

    while (1)
    {
        linea = readline("FIFO2B-OS>");

        if (linea)
        {
            add_history(linea);
        }

        int valido = verificar_comando(linea);

        if (valido > 0)
            atender_instruccion(linea);
    }
}

int verificar_comando(char *leido)
{
    char **comando = string_split(leido, " ");
    t_log *logger_comando = log_create("logs/kernel_info.log", "plani_cp", 1, LOG_LEVEL_INFO);

    if (
        strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ||
        strcmp(comando[0], "INICIAR_PROCESO") == 0 ||
        strcmp(comando[0], "FINALIZAR_PROCESO") == 0 ||
        strcmp(comando[0], "DETENER_PLANIFICACION") == 0 ||
        strcmp(comando[0], "INICIAR_PLANIFICACION") == 0 ||
        strcmp(comando[0], "MULTIPROGRAMACION") == 0 ||
        strcmp(comando[0], "PROCESO_ESTADO") == 0 ||
        strcmp(comando[0], "APAGAR_SISTEMA") == 0)
    {
        free(comando);
        log_destroy(logger_comando);
        return 1;
    }
    free(comando);
    log_error(logger_comando, "Comando no reconocido");
    log_destroy(logger_comando);
    return -1;
}

void atender_instruccion(char *leido)
{
    char **comando = string_split(leido, " ");

    if (strcmp(comando[0], "EJECUTAR_SCRIPT") == 0)
    {
        leer_script(comando[1]);
    }
    else if (strcmp(comando[0], "INICIAR_PROCESO") == 0)
    {
        char *path = comando[1];
        pthread_t hilo_creacion_proceso;
        pthread_create(&hilo_creacion_proceso, NULL, PLPNuevoProceso, path);
        pthread_join(hilo_creacion_proceso, NULL);
    }
    else if (strcmp(comando[0], "FINALIZAR_PROCESO") == 0) // FALTA HACER
    {
        detenerPlanificador();
        if (encontrar_y_terminar_proceso(atoi(comando[1])) < 0)
            encontrar_en_new_y_terminar(atoi(comando[1]));
        reanudarPlanificador();
    }
    else if (strcmp(comando[0], "DETENER_PLANIFICACION") == 0)
    {
        detenerPlanificador();
    }
    else if (strcmp(comando[0], "INICIAR_PLANIFICACION") == 0)
    {
        reanudarPlanificador();
    }
    else if (strcmp(comando[0], "MULTIPROGRAMACION") == 0)
    {
        ajustar_grado_multiprogramacion(atoi(comando[1]));
    }
    else if (strcmp(leido, "PROCESO_ESTADO") == 0)
    {
        detenerPlanificador();
        listar_por_estado();
        listar_estados_lp();
        reanudarPlanificador();
    }

    string_array_destroy(comando);
}

void leer_script(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1)
    {
        // Eliminar el salto de línea si está presente
        if (read > 0 && line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }
        atender_instruccion(line);
    }

    free(line);
    fclose(file);
}