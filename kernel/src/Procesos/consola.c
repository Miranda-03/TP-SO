#include "consola.h"

void consolaInteractiva()
{
    char *linea;

    while (1)
    {
        linea = readline("FIFO-OS>");

        if (!linea)
        {
            break;
        }

        int valido = verificar_comando(linea);
        
        if (valido < 0)
            printf("comando no reconocido\n");
        else
            atender_instruccion(linea);
    }
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
        strcmp(comando[0], "PROCESO_ESTADO")) == 0
    {
        return 1;
    }
    return -1;
}

void atender_instruccion(char *leido)
{
    char **comando = string_split(leido, " ");

    t_buffer *buffer = buffer_create(sizeof(t_buffer));

    if (strcmp(comando[0], "EJECUTAR_SCRIPT") == 0)
    {
        // Aquí puedes agregar el código para EJECUTAR_SCRIPT
    }
    else if (strcmp(comando[0], "INICIAR_PROCESO") == 0)
    {
        char *path = comando[1];
        pthread_t hilo_creacion_proceso;
        pthread_create(&hilo_creacion_proceso, NULL, PLPNuevoProceso, path);
        pthread_detach(hilo_creacion_proceso);
    }
    else if (strcmp(comando[0], "FINALIZAR_PROCESO") == 0) // FALTA HACER
    {
        /*
        int resultadoFinalizado = buscarProcesoEnREADYyEXITporIDyFinalizarlo(comando[1]);
        if (resultadoFinalizado < 0)
            buscarProcesoEnPlanificadorCP(comando[1]);
        */
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
    
    /*
    else if (strcmp(leido, "PROCESO_ESTADO") == 0)
    {
        // Aquí puedes agregar el código para PROCESO_ESTADO
    }
    else
    {
        log_error(logger_kernel, "ERROR");
    } */
}
