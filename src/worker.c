#include "worker.h"
#include "copias.h"
#include "memoriaCompartida.h"
#include "logger.h"
#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>

extern int ejecutando;

void iniciarWorker(int idWorker, int pipeLeer, MemoriaCompartida *memoria, sem_t *semaforo) {
    InfoArchivo archivo;

    mqd_t colaLogger = mq_open(NOMBRE_COLA, O_WRONLY);
    if (colaLogger == -1) {
        perror("Error al abrir la cola de mensajes para el logger");
        //continuar sin logger
    }

    while(ejecutando) {
        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(pipeLeer, &set);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int result = select(pipeLeer + 1, &set, NULL, NULL, &timeout);
        
        if (result > 0) {
            int bytesLeidos = read(pipeLeer, &archivo, sizeof(InfoArchivo));
            if (bytesLeidos > 0) {
                printf("Worker %d procesando archivo: %s\n", idWorker, archivo.ruta);
                
                if(archivo.permisos == 0) {
                    // Archivo eliminado
                    eliminarArchivoBackup(archivo, "backup");
                } else {
                    // Archivo nuevo o modificado
                    int exito = copiarArchivo(archivo, "backup");
                    sem_wait(semaforo);

                    if (exito == 0) {
                        memoria->archivosCopiados++;
                        memoria->bytesCopiados += archivo.tamano;
                        strcpy(memoria->ultimoArchivo, archivo.nombre);
                    
                        if (colaLogger != -1) {
                            char mensaje[256];
                            snprintf(mensaje, sizeof(mensaje), 
                                "Worker %d: Copiado archivo %s (tamano: %ld bytes)", 
                                idWorker, archivo.nombre, archivo.tamano);
                            mq_send(colaLogger, mensaje, strlen(mensaje) + 1, 0);
                        }
                    } else {
                        memoria->errores++;
                    
                        if (colaLogger != -1) {
                            char mensaje[256];
                            snprintf(mensaje, sizeof(mensaje), 
                                "Worker %d: Error al copiar archivo %s", 
                                idWorker, archivo.nombre);
                            mq_send(colaLogger, mensaje, strlen(mensaje) + 1, 0);
                        }
                    }
                    sem_post(semaforo);
                }
            }
        } else if (result == 0) {
            // Timeout - continuar
            continue;
        } else {
            perror("Error en select");
            break;
        }
    }
    
    printf("Worker %d: Cerrandose.\n", idWorker);
    
    if (colaLogger != -1) {
        mq_close(colaLogger);
    }
}