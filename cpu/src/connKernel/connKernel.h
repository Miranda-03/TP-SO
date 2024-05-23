#ifndef KERNEL_CONN_H
#define KERNEL_CONN_H

/**
* @fn    manageDISPATCH
* @brief Función en el hilo para la conexión dispatch con el Kernel.
*/
void * manageDISPATCH(void * ptr)

/**
* @fn    manageINTERRUPT
* @brief Función en el hilo para la conexión interrumpt con el Kernel.
*/
void * manageINTERRUPT(void * ptr)

#endif