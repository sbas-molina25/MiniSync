#include "memoriaCompartida.h"
#include <stdio.h>
#include <string.h>

#define NOMBRE_MEMORIA "/MiniSyncMemoria"

MemoriaCompartida *crearMemoriaCompartida() {
    int descriptor;
    descriptor = shm_open(NOMBRE_MEMORIA, O_CREAT | O_RDWR, 0666);

    if (descriptor == -1) {
        perror("Error al crear memoria compartida");
        return NULL;
    }
    if (ftruncate(descriptor, sizeof(MemoriaCompartida)) == -1) {
        perror("Error en ftruncate");
        close(descriptor);
        return NULL;
    }

    MemoriaCompartida *memoria;
    memoria = mmap(NULL, sizeof(MemoriaCompartida), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

    if (memoria == MAP_FAILED) {
        perror("Error en mmap");
        close(descriptor);
        return NULL;
    }
    close(descriptor);

    memoria->archivosCopiados = 0;
    memoria->bytesCopiados = 0;
    memoria->errores = 0;
    strcpy(memoria->ultimoArchivo, "");
    return memoria;
}

void cerrarMemoriaCompartida(MemoriaCompartida *memoria) {
    if (memoria != NULL && memoria != MAP_FAILED) {
        munmap(memoria, sizeof(MemoriaCompartida));
    }
}

void eliminarMemoriaCompartida() {
    shm_unlink(NOMBRE_MEMORIA);
}

void mostrarEstadisticas(MemoriaCompartida *memoria) {
    if (memoria == NULL) return;
    printf("\n========== ESTADÍSTICAS ===========\n");
    printf("Archivos copiados: %ld\n", memoria->archivosCopiados);
    printf("Bytes copiados: %ld\n", memoria->bytesCopiados);
    printf("Errores: %ld\n", memoria->errores);
    printf("Último archivo copiado: %s\n", memoria->ultimoArchivo);
    printf("=========================================\n\n");
}