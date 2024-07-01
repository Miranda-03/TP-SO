#include "mem_instrucciones.h"

t_dictionary *memoria_instrucciones;

int retardoEspera;

void crear_mem_instrucciones()
{
   memoria_instrucciones = dictionary_create();
   retardoEspera = atoi(obtenerValorConfig("memoria.config", "RETARDO_RESPUESTA"))/100;
}

int agregar_instrucciones(char *path, int pid)
{
   FILE *file_instrucciones;

   file_instrucciones = fopen(path, "r");

   if (file_instrucciones == NULL)
      return -1;

   char char_pid[10];

   snprintf(char_pid, sizeof(char_pid), "%d", pid);

   dictionary_put(memoria_instrucciones, char_pid, file_instrucciones);

   return 1;
}

char *obtener_instruccion(int pid, int pc)
{
   char char_pid[10];

   snprintf(char_pid, sizeof(char_pid), "%d", pid);

   if (!dictionary_has_key(memoria_instrucciones, char_pid))
      return "null";

   FILE *file = dictionary_get(memoria_instrucciones, char_pid);
   unsigned int contador = 0;
   char *instruccion = NULL;
   size_t len = 0;
   ssize_t read;
   while ((read = getline(&instruccion, &len, file)) != -1)
   {
      if (contador == pc)
      {
         // Eliminar el salto de línea si está presente
         if (read > 0 && instruccion[read - 1] == '\n')
         {
            instruccion[read - 1] = '\0';
         }

         rewind(file);
         sleep(retardoEspera);
         return instruccion;
      }
      contador++;
   }
   free(instruccion);
   return "null";
}