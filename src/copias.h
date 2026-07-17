#ifndef COPIAS_H
#define COPIAS_H
#include "scanner.h"

int copiarArchivo(InfoArchivo archivo, char *dirBackup);
int eliminarArchivoBackup(InfoArchivo archivo, char *dirBackup);

#endif