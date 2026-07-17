#ifndef WORKER_H
#define WORKER_H

#include "scanner.h"
#include "memoriaCompartida.h"
#include <semaphore.h>

void iniciarWorker(int idWorker, int pipeLeer, MemoriaCompartida *memoria, sem_t *semaforo);

#endif