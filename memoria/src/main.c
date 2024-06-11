#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <mem_instrucciones/mem_instrucciones.h>

int main(int argc, char* argv[]) {

    Memoria* mem = inicializar_memoria();

    crear_mem_instrucciones();
    conectarModuloMemoria();

    liberarMemoria(mem);

    return 0;
}
