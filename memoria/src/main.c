#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <mem_instrucciones/mem_instrucciones.h>

int main(int argc, char* argv[]) {

    Memoria* mem = inicializar_memoria();

    crear_mem_instrucciones();
    agregar_instrucciones(mem,"instruccionesPrueba.txt", 1200);
    //conectarModuloMemoria();
    liberar_memoria(mem);

    return 0;
}
