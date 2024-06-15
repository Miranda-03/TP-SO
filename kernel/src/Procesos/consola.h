#ifndef CONSOLA_H
#define CONSOLA_H

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <Conexion/conectar.h>
#include <utils/funcionesBuffer/funcionesBuffer.h>
#include <utils/structs/structProcesos.h>
#include <Planificadores/planificadorLP.h>
#include <readline/readline.h>

void consolaInteractiva();
int verificar_comando(char *leido);
void atender_instruccion(char* leido);


#endif