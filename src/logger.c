#include "logger.h"
#include <mqueue.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

void iniciarLogger() {
    struct mq_attr attr = {
        .mq_maxmsg = 10,
        .mq_msgsize = MAX_MENSAJE
    };

    mqd_t cola = mq_open(NOMBRE_COLA, O_CREAT | O_RDONLY, 0644, &attr);
    if (cola == -1) {
        perror("Error al crear la cola de mensajes");
        return;
    }

    char buffer[MAX_MENSAJE];
    while (1) {
        ssize_t bytesLeidos = mq_receive(cola, buffer, MAX_MENSAJE, NULL);
        if (bytesLeidos >= 0) {
            buffer[bytesLeidos] = '\0'; // Asegurarse de que el mensaje esté terminado en nulo
            time_t tiempoActual = time(NULL);
            struct tm *tm_info = localtime(&tiempoActual);
            char tiempoFormateado[20];
            strftime(tiempoFormateado, sizeof(tiempoFormateado), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("[%s] %s\n", tiempoFormateado, buffer);
            fflush(stdout); // escritura inmediata
        } else {
            perror("Error al recibir mensaje de la cola");
            break;
        }    
    }
    mq_close(cola);    
    mq_unlink(NOMBRE_COLA);
}