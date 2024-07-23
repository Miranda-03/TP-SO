#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <mem_instrucciones/mem_instrucciones.h>
#include <mem_usuario/mem_usuario.h>

char *path_config;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <ruta_archivo_configuracion>\n", argv[0]);
        return EXIT_FAILURE;
    }

    path_config = argv[1];

    iniciar_espacio_usuario(path_config);

    iniciar_marcos();

    iniciar_tablas_de_paginas();

    crear_mem_instrucciones(path_config);

    conectarModuloMemoria(path_config);

    return 0;
}
