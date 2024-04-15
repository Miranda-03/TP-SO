#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include<commons/config.h>
#include "utils.h"

void iterator(char* value);
t_config* iniciar_config(void);
t_log* iniciar_logger(void);

#endif /* SERVER_H_ */


