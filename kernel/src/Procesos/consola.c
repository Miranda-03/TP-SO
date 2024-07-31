#include "consola.h"

bool detenido_previamente;

void consolaInteractiva()
{
    detenido_previamente = 0;

    char *linea;

    while (1)
    {
        linea = readline("FIFO2B-OS>");

        if (!linea || strcmp(linea, "") == 0)
        {
            continue;
        }

        if (linea)
        {
            add_history(linea);

            int valido = verificar_comando(linea);

            if (valido > 0)
                atender_instruccion(linea);
        }
    }
}

int verificar_comando(char *leido)
{
    char **comando = string_split(leido, " ");
    t_log *logger_comando = log_create("logs/kernel_info.log", "consola", 1, LOG_LEVEL_INFO);

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
        string_array_destroy(comando);
        log_destroy(logger_comando);
        return 1;
    }
    string_array_destroy(comando);
    log_error(logger_comando, "Comando no reconocido");
    log_destroy(logger_comando);
    return -1;
}

void atender_instruccion(char *leido)
{
    char **comando = string_split(leido, " ");

    t_log *logger_consola = log_create("logs/kernel_info.log", "consola", 1, LOG_LEVEL_INFO);

    if (strcmp(comando[0], "EJECUTAR_SCRIPT") == 0)
    {
        leer_script(comando[1]);
    }
    else if (strcmp(comando[0], "INICIAR_PROCESO") == 0)
    {
        char *path = comando[1];
        PLPNuevoProceso(path);
    }
    else if (strcmp(comando[0], "FINALIZAR_PROCESO") == 0)
    {
        detenerPlanificador();
        bloquear_paso_de_procesos_a_colas();

        if (encontrar_y_terminar_proceso(atoi(comando[1])) < 0)
        {
            if (encontrar_en_new_y_terminar(atoi(comando[1])) < 0)
            {
                log_error(logger_consola, "No se pudo encontrar el proceso: %d\n", atoi(comando[1]));
            }
        }

        desbloquear_paso_de_procesos_a_colas();

        if (!detenido_previamente)
            reanudarPlanificador();
    }
    else if (strcmp(comando[0], "DETENER_PLANIFICACION") == 0)
    {
        detenerPlanificador();
        detenido_previamente = 1;
    }
    else if (strcmp(comando[0], "INICIAR_PLANIFICACION") == 0)
    {
        reanudarPlanificador();
        detenido_previamente = 0;
    }
    else if (strcmp(comando[0], "MULTIPROGRAMACION") == 0)
    {
        ajustar_grado_multiprogramacion(atoi(comando[1]));
    }
    else if (strcmp(leido, "PROCESO_ESTADO") == 0)
    {
        // detenerPlanificador();
        listar_por_estado();
        listar_estados_lp();
        // reanudarPlanificador();
    }

    string_array_destroy(comando);
    log_destroy(logger_consola);
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
