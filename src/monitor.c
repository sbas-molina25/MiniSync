#include "monitor.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "memoriaCompartida.h"
#include "scanner.h"

int siguienteWorker = 0;
extern int ejecutando;

void iniciarMonitor(char *rutaDir, int pipe1, int pipe2, MemoriaCompartida *memoria) {
    ListaArchivos listaAnterior, listaActual;
    inicializarLista(&listaAnterior);
    inicializarLista(&listaActual);
    escanearDir(rutaDir, &listaAnterior);
    int contador = 0;

    while(ejecutando) { //bucle que se ejecuta constantemente para monitorear los cambios
        sleep(5); // esperar 5 segundos para escanear de nuevo
        inicializarLista(&listaActual);
        escanearDir(rutaDir, &listaActual);
        compararListas(&listaAnterior, &listaActual, pipe1, pipe2);
        copiarListaArchivos(&listaAnterior, &listaActual); // Actualizar la lista anterior para la próxima comparación
        contador++;
        if (contador % 6 == 0) {
            mostrarEstadisticas(memoria);
        }
    }
}

void compararListas(ListaArchivos *listaAnterior, ListaArchivos *listaActual, int pipe1, int pipe2) {
    // Verificar archivos nuevos o modificados
    for (int i = 0; i < listaActual->cantidad; i++) {
        int indice = buscarArchivoEnLista(listaAnterior, listaActual->archivos[i]);
        if (indice == -1) {
            if (siguienteWorker == 0) {
                write(pipe1, &listaActual->archivos[i], sizeof(InfoArchivo));
                printf("Enviado a Worker 1: %s\n", listaActual->archivos[i].nombre);
                siguienteWorker = 1;
            } else {
                siguienteWorker = 0;
                write(pipe2, &listaActual->archivos[i], sizeof(InfoArchivo));
                printf("Enviado a Worker 2: %s\n", listaActual->archivos[i].nombre);
            }
        } else {
            if (listaActual->archivos[i].fechaModificacion != listaAnterior->archivos[indice].fechaModificacion || 
            listaActual->archivos[i].tamano != listaAnterior->archivos[indice].tamano) {
                if (siguienteWorker == 0) {
                    write(pipe1, &listaActual->archivos[i], sizeof(InfoArchivo));
                    printf("Archivo nuevo/modificado. Enviado a Worker 1: %s\n", listaActual->archivos[i].nombre);
                    siguienteWorker = 1;
                } else {
                    siguienteWorker = 0;
                    write(pipe2, &listaActual->archivos[i], sizeof(InfoArchivo));
                    printf("Archivo nuevo/modificado. Enviado a Worker 2: %s\n", listaActual->archivos[i].nombre);
                }
            }
        }
    }

    // Verificar archivos eliminados
    for (int i = 0; i < listaAnterior->cantidad; i++) {
        int indice = buscarArchivoEnLista(listaActual, listaAnterior->archivos[i]);
        if (indice == -1) {
            InfoArchivo archivoEliminado = listaAnterior->archivos[i];
            archivoEliminado.permisos = 0; // Indicar que el archivo ha sido eliminado
            if (siguienteWorker == 0) {
                    write(pipe1, &archivoEliminado, sizeof(InfoArchivo));
                    printf("Archivo eliminado. Enviado a Worker 1: %s\n", archivoEliminado.nombre);
                    siguienteWorker = 1;
                } else {
                    siguienteWorker = 0;
                    write(pipe2, &archivoEliminado, sizeof(InfoArchivo));
                    printf("Archivo eliminado. Enviado a Worker 2: %s\n", archivoEliminado.nombre);
                }
        }
    }
}

int buscarArchivoEnLista(ListaArchivos *lista, InfoArchivo archivo) {
    for (int i = 0; i < lista->cantidad; i++) {
        if (strcmp(lista->archivos[i].ruta, archivo.ruta) == 0) {
            return i; //retorna el índice del archivo encontrado
        }
    }
    return -1; //retorna -1 si no se encuentra el archivo
}