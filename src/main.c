#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "monitor.h"
#include "worker.h"
#include "memoriaCompartida.h"
#include "semaforo.h"
#include "logger.h"

//Para compilar: gcc src/*.c -o minisync 
//Para ejecutar: ./minisync <ruta_del_directorio>

int ejecutando = 1;

void manejarSenales(int senal) {
    if (senal == SIGINT || senal == SIGTERM) {
        ejecutando = 0;
        printf("Señal recibida, terminando procesos...\n");
        fflush(stdout); // Asegurarse de que el mensaje se imprima antes de salir
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <ruta_del_directorio>\n", argv[0]);
        return 1;
    }

    /* // Crear daemon
    pid_t daemon = fork();
     if (daemon < 0) {
        perror("Error al crear el daemon");
        return 1;
    }
    
    if (daemon > 0) {
        // Proceso padre: termina
        printf("Daemon iniciado con PID: %d\n", daemon);
        return 0;
    }
    
    // Proceso hijo: se convierte en daemon
    setsid();  // Nueva sesión
    
    // Cambiar al directorio raíz
    if (chdir("/") < 0) {
        perror("Error al cambiar directorio");
        return 1;
    }
    
    // Cerrar descriptores estándar
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);*/
    
    // Crear memoria compartida
    MemoriaCompartida *memoria = crearMemoriaCompartida();
    if (memoria == NULL) {
        return 1;
    }

    // Crear semáforo
    sem_t *semaforo = crearSemaforo();
    if (semaforo == NULL) {
        cerrarMemoriaCompartida(memoria);
        eliminarMemoriaCompartida();
        return 1;
    }    

    // Crear pipe para comunicación entre monitor y worker
    int pipe1[2];
    int pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) { 
        perror("Error al crear los pipe");
        return 1;
    }
  
    //Crear logger
    pid_t logger = fork();
    if (logger < 0) {
        perror("Error al crear el logger");
        return 1;
    }
    
    if (logger == 0) {
        // Proceso hijo: Logger
        mq_unlink(NOMBRE_COLA);
        iniciarLogger();
        return 0;
    }

    //Worker 1
    pid_t worker1 = fork();
    if (worker1 == -1) {
        perror("Error al crear el fork worker1");
        return 1;
    } else if (worker1 == 0) { //worker1
        close(pipe1[1]); //no escribe
        close(pipe2[0]); //no lee
        close(pipe2[1]); //no escribe
        iniciarWorker(1, pipe1[0], memoria, semaforo);
        return 0;
    } 

    //Worker 2
    pid_t worker2 = fork();
    if (worker2 == -1) {
        perror("Error al crear el fork worker2");
        return 1;
    } else if (worker2 == 0) { //worker2
        close(pipe2[1]); //no escribe
        close(pipe1[0]); //no lee
        close(pipe1[1]); //no escribe
        iniciarWorker(2, pipe2[0], memoria, semaforo);
        return 0;
    }      

    //Padre = monitor
    close(pipe1[0]); 
    close(pipe2[0]); 
    signal(SIGINT, manejarSenales);
    signal(SIGTERM, manejarSenales);
    printf("=== MINISYNC INICIADO ===\n");
    printf("Monitoreando: %s\n", argv[1]);
    printf("Presiona Ctrl+C para detener\n");
    printf("==========================\n\n");
    iniciarMonitor(argv[1], pipe1[1], pipe2[1], memoria);

    //Esperar a que los procesos hijos terminen
    waitpid(worker1, NULL, 0);
    waitpid(worker2, NULL, 0);
    waitpid(logger, NULL, 0);

    // Cerrar semáforo y memoria compartida al finalizar
    cerrarSemaforo(semaforo);
    eliminarSemaforo();
    cerrarMemoriaCompartida(memoria);
    eliminarMemoriaCompartida();

    printf("=== MINISYNC TERMINADO ===\n");
    return 0;
}