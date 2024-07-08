#include "mmu.h"

void mmu(){
 /*int mmu(Registro *reg, char tipo){
    if (tipo == 'i')
            reg->
        else if (tipo == 'u')
            reg->
}*/
}

/*DireccionLogica obtener_direccion_logica(int direccion_logica) {
    DireccionLogica dir;
    dir.numero_pagina = floor(direccion_logica / TAMANO_PAGINA);
    dir.desplazamiento = direccion_logica % TAMANO_PAGINA;
    return dir;
}

int traducir_direccion(DireccionLogica dir_logica) {
    int marco = tabla_paginas[dir_logica.numero_pagina].marco;
    if (marco == -1 || !tabla_paginas[dir_logica.numero_pagina].presente) {
        printf("Fallo de página!\n");
        exit(1);  // Manejo de fallos de página
    }
    return marco * TAMANO_PAGINA + dir_logica.desplazamiento;
} */