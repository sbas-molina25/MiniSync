#ifndef SEMAFORO_H
#define SEMAFORO_H
#include <semaphore.h>

sem_t *crearSemaforo();
void cerrarSemaforo(sem_t *semaforo);
void eliminarSemaforo();

#endif