#include "memoriaPrincipal.h"

t_log* logger_memoria;
t_config* config_memoria;

int tam_memoria;
int tam_pagina; 
int tam_frame;
int cant_paginas;


Memoria* inicializar_memoria() {
    
    logger_memoria = log_create("memoria.log","Logger_Memoria",1,LOG_LEVEL_INFO);
    config_memoria = config_create("memoria.config");
    
    if (config_memoria == NULL) {
        log_error(logger_memoria, "No se pudo cargar el archivo de configuración.");
        exit(1);
    }

    tam_memoria = config_get_int_value(config_memoria, "TAM_MEMORIA");
    tam_pagina  = config_get_int_value(config_memoria, "TAM_PAGINA");
    tam_frame = tam_pagina;
    cant_paginas = tam_memoria/tam_pagina;

    Memoria* mem = (Memoria*)malloc(sizeof(Memoria));


    mem->espacio_usuario = malloc(tam_memoria);
    if (mem->espacio_usuario == NULL) {
        log_error(logger_memoria,"No hay espacio para hacer el malloc de la memoria");
        exit(1);
    }

    mem->tabla_paginas = dictionary_create();
     if (mem->tabla_paginas == NULL) {
        log_error(logger_memoria,"No hay espacio para hacer la tabla de paginas");
        exit(1);
    }
    inicializar_frames(mem);
    log_info(logger_memoria, "Memoria inicializada correctamente");

    return mem;
}

void liberar_memoria(Memoria* mem) {
    if (mem != NULL) {
        dictionary_destroy_and_destroy_elements(mem->tabla_paginas, free);
        free(mem->espacio_usuario);
        free(mem);
    }
    else{
        log_info(logger_memoria,"Puntero a memoria esta en NULL");
    }



    log_destroy(logger_memoria);
    config_destroy(config_memoria);
}

void* leer_desde_usuario(Memoria* memoria, int direccion_fisica, int bytes_lectura) {
       
    int pagina_inicial = direccion_fisica / tam_pagina;
    int offset = direccion_fisica % tam_pagina;
    int bytes_leidos = 0;
    void* resultado = malloc(bytes_lectura);
    // int marco = falta como obtenemos el marco y el pid
    int pid;


    while (bytes_leidos < bytes_lectura) {
        // int marco = falta como obtenemos el marco

        int bytes_disponibles = tam_pagina - offset;
        int bytes_a_copiar = bytes_lectura - bytes_leidos;
        int bytes_a_leer;
        int bytes_a_escribir;

        if (bytes_disponibles < bytes_a_copiar) {
            bytes_a_escribir = bytes_disponibles;
        }else {
            bytes_a_escribir = bytes_a_copiar;
        } 

        log_info(logger_memoria, "Acceso a Espacio de Usuario: PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño %d bytes", 
        pid, direccion_fisica);

       
        //memcpy(la informacion para leer);
        bytes_leidos += bytes_a_leer;

        pagina_inicial++;
        offset = 0;
    }

    return resultado;
}

int escribir_en_usuario(Memoria* memoria, int direccion_fisica, void* datos, int tamano_escritura) {    
    
    int pagina_inicial = direccion_fisica / tam_pagina;
    int offset = direccion_fisica % tam_pagina;
    int bytes_escritos = 0;

    
    while (bytes_escritos < tamano_escritura && pagina_inicial < cant_paginas) {
        
        Frame* frame = (Frame*) dictionary_get(memoria->tabla_paginas, pagina_inicial);
        if (frame == NULL || frame->presencia == 0) {
            return -1; // La página no está presente en memoria
        }

  
        int bytes_disponibles = tam_pagina - offset;
        int bytes_a_copiar = tamano_escritura - bytes_escritos;
        int bytes_a_escribir = tam_pagina - offset;

        
        //memcpy(la informacion para escribir);
        bytes_escritos += bytes_a_escribir;
        pagina_inicial++;
        offset = 0; 
    }
    return 1;
}

bool es_frame_libre(void* elem) {
    Frame* frame = (Frame*)elem;
    return frame->presencia == 0;
}

void asignar_espacio(Memoria *mem, char char_pid[5])
 {
    t_list* Lframes=dictionary_elements(mem->tabla_paginas);
    Frame* frame = list_find(Lframes, es_frame_libre);

    if (frame == NULL) {
        fprintf(stderr, "No se encontro un marco disponible\n");
        list_destroy(Lframes);
        return;
    }
    
    int pid = atoi(char_pid);
    
    Frame* frame_nuevo = malloc(sizeof(Frame));
    frame_nuevo->pagina=frame->pagina;
    frame_nuevo->pid=pid;
    frame_nuevo->presencia=1;
    dictionary_put(mem->tabla_paginas,frame->pagina,frame_nuevo);
    list_destroy(Lframes);
 }

 void inicializar_frames(Memoria* mem) {
    
    for (int i = 0; i < 256; i++) {  
        Frame *frame = (Frame*)malloc(sizeof(Frame));

        frame->pagina = i;
        frame->pid = 0;
        frame->presencia = 0;

        char key[10];
        sprintf(key, "%d", i);
        dictionary_put(mem->tabla_paginas, key, frame);
        free(frame);
    }
}