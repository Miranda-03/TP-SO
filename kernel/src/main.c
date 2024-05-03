#include <stdlib.h>
#include <stdio.h>
#include <Conexion/conectar.h>
#include <Procesos/procesos.h>

int *socketIO=1;

int main(int argc, char* argv[]) {

    conectarModuloKernel();
    planificaciónDeProcesos();
    
    return 0;
}