#include <utils/structs/structProcesos.h>

void encolar(ColaDeProcesos **pri, ColaDeProcesos **ult, Proceso proceso)
{
    ColaDeProcesos *p = (ColaDeProcesos *)malloc(sizeof(ColaDeProcesos));
    if (p == NULL) {
        fprintf("Error: No se pudo asignar memoria.\n");
        exit(1);
    }
    p->info = proceso;
    p->sig = NULL;
    if (*ult != NULL)
        (*ult)->sig = p;
    else
        *pri = p;
    *ult = p;
}   

void ordenarproceso(Proceso proceso){

}