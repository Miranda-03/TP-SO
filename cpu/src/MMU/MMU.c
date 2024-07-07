#include "MMU.h"

int tam_pagina;

void instanciar_tam_pagina_MMU(int tam)
{
    tam_pagina = tam;
}

int traducir_direccion(int num_pagina, int desplazamiento, int pid)
{
    int marco = obtener_marco(num_pagina, pid);

    return marco * tam_pagina + desplazamiento;
}

char **obtener_direcciones_fisicas(int direccion_logica, int bytes, int pid)
{
    /*
        Guarda las direcciones fisicas calculadas en un array de chars. Se guarda la direccion seguido
        de los bytes que se usan a partir de esa direccion fisica.
    */

    int numero_pagina = floor(direccion_logica / tam_pagina);
    int desplazamiento = direccion_logica - numero_pagina * tam_pagina;

    char **direcciones_fisicas = string_array_new();

    int dir_fisica_inicial = traducir_direccion(numero_pagina, desplazamiento, pid);

    string_array_push(&direcciones_fisicas, string_itoa(dir_fisica_inicial));

    if (desplazamiento + bytes > tam_pagina)
    {
        int bytes_a_leer_en_la_primera_pagina = tam_pagina - desplazamiento;

        string_array_push(&direcciones_fisicas, string_itoa(bytes_a_leer_en_la_primera_pagina));

        bytes -= bytes_a_leer_en_la_primera_pagina;
    }
    else
    {
        string_array_push(&direcciones_fisicas, string_itoa(bytes));
        bytes = 0;
    }

    while (bytes != 0)
    {
        numero_pagina += 1;
        string_array_push(&direcciones_fisicas, string_itoa(traducir_direccion(numero_pagina, 0, pid)));
        if (bytes >= tam_pagina)
        {
            string_array_push(&direcciones_fisicas, string_itoa(tam_pagina));
            bytes -= tam_pagina;
        }
        else
        {
            string_array_push(&direcciones_fisicas, string_itoa(bytes));
            bytes = 0;
        }
    }

    return direcciones_fisicas;
}