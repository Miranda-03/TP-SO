#include "Globales/globales.h"




int main(int argc, char* argv[]) {
    
    
    initialize(); // esto se puede cambiar
    conectarModuloKernel();
    consolaInteractiva();

    log_debug(logger_kernel, "Salida del Kernel");
    return 0;
}


