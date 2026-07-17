#ifndef LOGGER_H
#define LOGGER_H

#include <mqueue.h>
#define NOMBRE_COLA "/MiniSyncLogger"
#define MAX_MENSAJE 256

void iniciarLogger();

#endif 