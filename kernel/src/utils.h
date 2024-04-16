#ifndef UTILS_H_
#define UTILS_H_

#include <utils/structs/structSendRecvMSG.h>
#include <utils/enums/DispositivosIOenum.h>
#include <utils/crearConexiones/crearConexiones.h>

/**
* @fn    manageIO
* @brief manejar el handshake del modulo IO.
*/
void *atenderIo(void *socketComunicacion);


#endif /* UTILS_H_ */
