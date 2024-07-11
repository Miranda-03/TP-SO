#include "mem_instrucciones.h"

t_dictionary *memoria_instrucciones;

int retardoEspera;

void crear_mem_instrucciones()
{
   memoria_instrucciones = dictionary_create();
   retardoEspera = atoi(obtenerValorConfig("memoria.config", "RETARDO_RESPUESTA"));
}

int agregar_instrucciones(char *path, int pid)
{
   FILE *file_instrucciones;

   file_instrucciones = fopen(path, "r");

   if (file_instrucciones == NULL)
      return -1;
   
   fclose(file_instrucciones);

   char char_pid[10];

   snprintf(char_pid, sizeof(char_pid), "%d", pid);

   dictionary_put(memoria_instrucciones, char_pid, path);

   return 1;
}

int quitar_instrucciones(int pid)
{
   char char_pid[10];

   snprintf(char_pid, sizeof(char_pid), "%d", pid);

   if(!dictionary_has_key(memoria_instrucciones, char_pid))
      return -1;

   dictionary_remove(memoria_instrucciones, char_pid);

   return 1;
}

char *obtener_instruccion(int pid, int pc)
{
   char char_pid[10];

   snprintf(char_pid, sizeof(char_pid), "%d", pid);

   if (!dictionary_has_key(memoria_instrucciones, char_pid))
      return "null";

   char *path = dictionary_get(memoria_instrucciones, char_pid);

   FILE *file_instrucciones;

   file_instrucciones = fopen(path, "r");

   unsigned int contador = 0;
   char *instruccion = NULL;
   size_t len = 0;
   ssize_t read;
   while ((read = getline(&instruccion, &len, file_instrucciones)) != -1)
   {
      if (contador == pc)
      {
         // Eliminar el salto de línea si está presente
         if (read > 0 && instruccion[read - 1] == '\n')
         {
            instruccion[read - 1] = '\0';
         }

         rewind(file_instrucciones);
         fclose(file_instrucciones);
         usleep(retardoEspera * 1000);
         return instruccion;
      }
      contador++;
   }
   free(instruccion);
   return "null";
}

void enviar_instruccion(int *socket, int pc, int pid)
{
   char *instruccion = obtener_instruccion(pid, pc);
   t_buffer *buffer = buffer_create(sizeof(uint32_t) + (strlen(instruccion)) + 1);
   buffer_add_string(buffer, (strlen(instruccion)) + 1, instruccion);
   enviarMensaje(socket, buffer, MEMORIA, MENSAJE);
}