#include "utils.h"

void manageIO(int *socket, t_buffer *buffer, t_resultHandShake *result)
{

  void *stream = buffer->stream;

  TipoInterfaz tipo;
  memcpy(&tipo, stream, 4);

  switch (tipo)
  {
  case STDIN:
    enviarPaqueteResult(result, 0, socket);
    break;

  case STDOUT:
    enviarPaqueteResult(result, 0, socket);
    break;

  case DIALFS:
    enviarPaqueteResult(result, 0, socket);
    break;

  default:
    enviarPaqueteResult(result, -1, socket);
    break;
  }
}