#include "copias.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

void crearDirectorioBackup(char *dirBackup) {
    struct stat st;
    if (stat(dirBackup, &st) == -1) {
        mkdir(dirBackup, 0755);
        printf("Directorio backup creado: %s\n", dirBackup);
    }
}

int copiarArchivo(InfoArchivo archivo, char *dirBackup) {
    int archivoOrigen, archivoDestino;
    crearDirectorioBackup(dirBackup);

    char rutaDestino[512];
    char buffer[1024];
    ssize_t bytesLeidos, bytesEscritos;

    char *nombreArchivo = strrchr(archivo.ruta, '/');
    if (nombreArchivo != NULL) {
        nombreArchivo++; // Mover el puntero al inicio del nombre del archivo
    } else {
        nombreArchivo = archivo.ruta; // Si no hay '/', usar la ruta completa     
    }
    snprintf(rutaDestino, sizeof(rutaDestino), "%s/%s", dirBackup, nombreArchivo);

    archivoOrigen = open(archivo.ruta, O_RDONLY);
    if (archivoOrigen == -1) {
        perror("Error al abrir el archivo de origen");
        // REVISAR: no vale hacer el backup cuando se elimina del test
        return -1;
    }

    archivoDestino = open(rutaDestino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (archivoDestino == -1) {
        perror("Error al crear el archivo de destino");
        close(archivoOrigen);
        return -1;
    }

    while ((bytesLeidos = read(archivoOrigen, buffer, sizeof(buffer))) > 0) {
        bytesEscritos = write(archivoDestino, buffer, bytesLeidos);
        if (bytesEscritos != bytesLeidos) {
            perror("Error al escribir en el archivo de destino");
            close(archivoOrigen);
            close(archivoDestino);
            return -1;
        }
    }    

    close(archivoOrigen);
    close(archivoDestino);
    return 0;
}    

int eliminarArchivoBackup(InfoArchivo archivo, char *dirBackup) {
    char rutaDestino[512];
    char *nombreArchivo = strrchr(archivo.ruta, '/');
    if (nombreArchivo != NULL) {
        nombreArchivo++;
    } else {
        nombreArchivo = archivo.ruta;
    }
    snprintf(rutaDestino, sizeof(rutaDestino), "%s/%s", dirBackup, nombreArchivo);
    
    if (unlink(rutaDestino) == -1) {
        perror("Error al eliminar archivo de backup");
        return -1;
    } else {
        printf("Archivo eliminado de backup: %s\n", rutaDestino);
        return 0;
    }
    return 0;
}