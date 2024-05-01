#include "utils/obtenerValorConfig/obtenerValorConfig.h"

char *obtenerValorConfig(char *path, char *key)
{
    char *valor;
    t_config *config = config_create(path);

    if (config_has_property(config, key))
        valor = strdup(config_get_string_value(config, key)); 

    config_destroy(config);

    return valor;
}

