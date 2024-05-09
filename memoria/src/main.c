#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <mem_instrucciones/mem_instrucciones.h>

int main(int argc, char* argv[]) {

    crear_mem_instrucciones();
    
    conectarModuloMemoria();

    return 0;
}
