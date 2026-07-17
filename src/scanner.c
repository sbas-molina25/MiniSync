#include "scanner.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

void escanearDir(char *rutaDir, ListaArchivos *lista) {
    DIR *directorio; // puntero al directorio
    struct dirent *entrada; // puntero para leer los elementos

    directorio = opendir(rutaDir); // abrir el directorio
    if (directorio == NULL) {
        perror("No se pudo abrir el directorio"); //print error si no se abre
        return;
    }

    while ((entrada = readdir(directorio)) != NULL) { // leer cada elemento del directorio
        char rutaCompleta[512];
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }
        snprintf(rutaCompleta, sizeof(rutaCompleta), "%s/%s", rutaDir, entrada->d_name);
        procesarEntrada(rutaCompleta, entrada->d_name, lista);
    }
    closedir(directorio);
}   

void procesarEntrada(char *rutaCompleta, char *nombreArchivo, ListaArchivos *lista) {
    struct stat info;
    if (stat(rutaCompleta, &info) == -1){
        perror("stat");
        return;
    }

    if (S_ISDIR(info.st_mode)) { // si es directorio
        //printf("\n DIRECTORIO: %s\n", rutaCompleta);
        escanearDir(rutaCompleta, lista); // llamada recursiva para escanear el subdirectorio
    } else if (S_ISREG(info.st_mode)) { // si es solo un archivo
        InfoArchivo archivo;
        snprintf(archivo.nombre, sizeof(archivo.nombre), "%s", nombreArchivo);
        snprintf(archivo.ruta, sizeof(archivo.ruta), "%s", rutaCompleta);
        archivo.inodo = info.st_ino;
        archivo.tamano = info.st_size;
        archivo.permisos = info.st_mode & 0777;
        archivo.fechaModificacion = info.st_mtime;
        agregarArchivo(lista, archivo);
    } 
}

void agregarArchivo(ListaArchivos *lista, InfoArchivo archivo) {
    if (lista->cantidad < 1000) {
        lista->archivos[lista->cantidad] = archivo;
        lista->cantidad++;
    } else {
        printf("La lista de archivos está llena.\n");
    }
}

void copiarListaArchivos(ListaArchivos *destino, ListaArchivos *origen) {
    destino->cantidad = origen->cantidad;
    for (int i = 0; i < origen->cantidad; i++) {
        destino->archivos[i] = origen->archivos[i];
    }
}

void inicializarLista(ListaArchivos *lista){
    lista->cantidad = 0;
}

void imprimirInfoArchivo(InfoArchivo *archivo) {
    printf("\n==========================================\n");
    printf("Nombre del Archivo: %s\n", archivo->nombre);
    printf("Ruta: %s\n", archivo->ruta);
    printf("Inodo: %ld\n", archivo->inodo);
    printf("Tamaño: %ld bytes\n", archivo->tamano);
    printf("Permisos: %o\n", archivo->permisos);
    printf("Fecha de Modificación: %ld\n", archivo->fechaModificacion);
}