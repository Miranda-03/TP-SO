#include <Conexion/conectar.h>
#include "utils/crearIO.h"

int main(){
    
    inicializarMutex();

    crearIO("io_config/impresora.config", "impresora");

    crearIO("io_config/teclado.config", "teclado");

    crearIO("io_config/monitor.config", "monitor");    

    return 0;
}

