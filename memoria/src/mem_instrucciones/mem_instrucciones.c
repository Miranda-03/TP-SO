#include "mem_instrucciones.h"

t_dictionary *memoria_instrucciones;

void crear_mem_instrucciones()
{
   memoria_instrucciones = dictionary_create();
}

int agregar_instrucciones(char *path, int pid)
{
   FILE *file_instrucciones;

   file_instrucciones = fopen(path, "r");

   if (file_instrucciones == NULL)
      return -1;

   char char_pid[10];
   sprintf(char_pid, "%d", pid);
   dictionary_put(memoria_instrucciones, &char_pid, file_instrucciones);

   return 1;
}

char *obtener_instruccion(unsigned int pid, unsigned int pc)
{
   char char_pid[10];
   sprintf(char_pid, "%d", pid);
   if (!dictionary_has_key(memoria_instrucciones, char_pid))
      return -1;

   FILE *file = dictionary_get(memoria_instrucciones, char_pid);
   unsigned int contador = 0;
   char *instruccion = NULL;
   size_t len = 0;
   while ((getline(&instruccio, &len, file)) != -1)
   {
      if (contador == pc) return instruccion;
      contador++;
   }
   return -1;
}