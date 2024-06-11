#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/structs/structProcesos.h>
#include "Globales/globales.h"
#include "procesos.h"
#include <utils/structs/structProcesos.h>

void consolaInteractiva();
bool verificar_comando(char* leido);
void atender_instruccion(char* leido);
