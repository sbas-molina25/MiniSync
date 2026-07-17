#ifndef MONITOR_H
#define MONITOR_H
#include "scanner.h"
#include "memoriaCompartida.h"

void iniciarMonitor(char *rutaDir, int pipe1, int pipe2, MemoriaCompartida *memoria);
void compararListas(ListaArchivos *listaAnterior, ListaArchivos *listaActual, int pipe1, int pipe2);
int buscarArchivoEnLista(ListaArchivos *lista, InfoArchivo archivo);

#endif 