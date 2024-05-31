#include <utils/structs/structProcesos.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesbBuffer.h>

void consolaInteractiva();
bool verificar_comando(char* leido);
void atender_instruccion(char* leido);