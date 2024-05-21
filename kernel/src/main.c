#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <Procesos/procesos.h>
#include <Procesos/consola.h>
#include <utils/logs.h>


int main(int argc, char* argv[]) {

    conectarModuloKernel();
    consolaInteractiva();

    return 0;
}