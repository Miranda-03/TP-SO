#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <mem_instrucciones/mem_instrucciones.h>
#include <mem_usuario/mem_usuario.h>

int main(int argc, char* argv[]) {

    iniciar_espacio_usuario();

    iniciar_marcos();

    iniciar_tablas_de_paginas();

    crear_mem_instrucciones();
    
    conectarModuloMemoria();

    return 0;
}
