#include "utils/obtenerValorConfig/obtenerValorConfig.h"

char *obtenerValorConfig(char *path, char *key)
{
    char *valor = string_new();
    t_config* config = config_create(path);

    if (config != NULL) {
        if (config_has_property(config, key)) {
            char* temp = config_get_string_value(config, key);
            if (temp != NULL) {
                string_append(&valor, temp);
                if (valor == NULL) {
                    // Manejar el error de asignación de memoria si es necesario
                }
            }
        }
        config_destroy(config);
    }
    
    return valor;
}

