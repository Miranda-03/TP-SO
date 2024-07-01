#include <Conexion/conectar.h>
#include "utils/crearIO.h"

int main()
{

    inicializarMutex();

    pthread_t hilos_de_escucha[2];

    crearIO("io_config/impresora.config", "impresora", &hilos_de_escucha[0]);

    //crearIO("io_config/teclado.config", "teclado", &hilos_de_escucha[1]);

    //crearIO("io_config/monitor.config", "monitor", &hilos_de_escucha[2]);

    crearIO("io_config/escaner.config", "escaner", &hilos_de_escucha[1]);

    for (int i = 0; i < 2; i++)
    {
        pthread_join(hilos_de_escucha[i], NULL);
    }

    return 0;
}
