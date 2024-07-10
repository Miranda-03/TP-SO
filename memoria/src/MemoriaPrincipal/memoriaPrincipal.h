#include<utils/structs/structsMemoria.h>
#include <utils/globales/loggers.h>
#include <utils/globales/config.h>


Memoria* inicializar_memoria();
void liberar_memoria(Memoria* mem);
void* leer_desde_usuario(Memoria* memoria, int direccion_fisica, int bytes_lectura);
int escribir_en_usuario(Memoria* memoria, int direccion_fisica, void* datos, int tamano_escritura);
bool es_frame_libre(void* elem);
void asignar_espacio(Memoria *mem, char char_pid[5]);
void inicializar_frames(Memoria* mem);