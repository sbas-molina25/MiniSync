#ifndef MEMORIACOMPARTIDA_H
#define MEMORIACOMPARTIDA_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    long archivosCopiados;
    long bytesCopiados;
    long errores;
    char ultimoArchivo[256];
} MemoriaCompartida;

MemoriaCompartida *crearMemoriaCompartida();
void cerrarMemoriaCompartida(MemoriaCompartida *memoria);
void eliminarMemoriaCompartida();
void mostrarEstadisticas(MemoriaCompartida *memoria);

#endif