/* #include <Procesos/consola.h>


void consolaInteractiva(){
    char* leido;
    leido = readline(">");
    bool valido;

    while(strcmp(leido,"exit") != 0){
        valido = verificar_comando(leido);
        if(!valido){
            log_error(logger_kernel,"Comando no reconocido");  
        }
        else {
            atender_instruccion(leido);   
        }
        free(leido);
        leido = readline(">");
    }
}

bool verificar_comando(char* leido)
{
    bool resultado = false;
    char** comando = string_split(leido," ");

    if (
    strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ||    
    strcmp(comando[0], "INICIAR_PROCESO") == 0 ||
    strcmp(comando[0], "FINALIZAR_PROCESO") == 0 ||
    strcmp(comando[0], "DETENER_PLANIFICACION") == 0 ||
    strcmp(comando[0], "INICIAR_PLANIFICACION") == 0 ||
    strcmp(comando[0], "MULTIPROGRAMACION") == 0 ||
    strcmp(comando[0], "PROCESO_ESTADO") 
    ) {
        resultado = true;
    } 
    return resultado;
}  

void atender_instruccion(char* leido){
    char** comando = string_split(leido," ");
    
    t_buffer* buffer= buffer_create(sizeof(t_buffer));

    if(strcmp(leido, "EJECUTAR_SCRIPT")==0)
    {  
        //buffer_add_string(buffer,sizeof(comando[1]),comando[1]);

    }
    else if(strcmp(leido, "INICIAR_PROCESO")==0)
    {
        buffer_add_string(buffer,sizeof(comando[1]),comando[1]);
        planificacionDeProcesos(1);
        //enviar
        
    }else if(strcmp(leido, "FINALIZAR_PROCESO")==0)
    {

    }else if(strcmp(leido, "DETENER_PLANIFICACION")==0)
    {

    }else if(strcmp(leido, "INICIAR_PLANIFICACION")==0)
    {

    }else if(strcmp(leido, "MULTIPROGRAMACION")==0)
    {

    }else if(strcmp(leido, "PROCESO_ESTADO")==0)
    {


    }else{
        log_error(logger_kernel,"ERROR");
        
    }
   
}
 */
