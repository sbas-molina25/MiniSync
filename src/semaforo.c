#include "semaforo.h"
#include <stdio.h>
#include <fcntl.h>

#define NOMBRE_SEMAFORO "/MiniSyncSemaforo"

sem_t *crearSemaforo() {
    sem_t *semaforo = sem_open(NOMBRE_SEMAFORO, O_CREAT, 0666, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al crear semáforo");
        return NULL;
    }
    return semaforo;
}

void cerrarSemaforo(sem_t *semaforo) {
    sem_close(semaforo);
}

void eliminarSemaforo() {
    sem_unlink(NOMBRE_SEMAFORO);
}