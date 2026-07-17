#ifndef SCANNER_H
#define SCANNER_H
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>

typedef struct {
    char nombre[256];
    char ruta[512];
    ino_t inodo;
    off_t tamano;
    mode_t permisos;
    time_t fechaModificacion;
} InfoArchivo;

typedef struct {
    InfoArchivo archivos[1000];
    int cantidad;
} ListaArchivos;

void escanearDir(char *rutaDir,  ListaArchivos *lista);
void procesarEntrada(char *rutaCompleta, char *nombreArchivo, ListaArchivos *lista);
void inicializarLista(ListaArchivos *lista);
void agregarArchivo(ListaArchivos *lista, InfoArchivo archivo);
void copiarListaArchivos(ListaArchivos *destino, ListaArchivos *origen);
void imprimirInfoArchivo(InfoArchivo *archivo);

#endif 