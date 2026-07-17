# MiniSync
MiniSync es una aplicación desarrollada en lenguaje C para Linux que realiza la sincronización de archivos entre un directorio de origen y un directorio de respaldo (backup/). El proyecto implementa conceptos fundamentales como procesos, comunicación entre procesos (IPC), memoria compartida, semáforos y monitoreo del sistema de archivos.

## Características
- Escaneo recursivo de directorios.
- Obtención de metadatos de archivos:
  - Nombre
  - Ruta
  - Número de i-nodo
  - Tamaño
  - Permisos
  - Fecha de modificación
- Sincronización incremental.
- Copia de archivos utilizando únicamente:
  - `open()`
  - `read()`
  - `write()`
- Monitor que detecta cambios periódicamente.
- Dos procesos Worker creados mediante `fork()`.
- Comunicación Monitor → Workers mediante Pipes.
- Memoria compartida para estadísticas.
- Protección de estadísticas mediante Semáforos POSIX.
- Servicio de Logging mediante un proceso independiente.
- Directorio de respaldo automático (`backup/`).

## Estructura del proyecto
MiniSync/
├── src/
│   ├── main.c
│   ├── scanner.c
│   ├── monitor.c
│   ├── worker.c
│   ├── copias.c
│   ├── memoriaCompartida.c
│   ├── semaforo.c
│   │── logger.c
│   ├── scanner.h
│   ├── monitor.h
│   ├── worker.h
│   ├── copias.h
│   ├── memoriaCompartida.h
│   ├── semaforo.h
│   └── logger.h
├── test/
├── Makefile
└── README.md

## Requisitos
- Ubuntu
- GCC
- Make
- Librerías POSIX

## Compilación
Si se utiliza Makefile:
```bash
make
```

O manualmente:
```bash
gcc src/*.c -o minisync -lrt -pthread
```

## Ejecución
Si se utiliza Makefile:
```bash
make run
```

El programa escaneará el directorio test/origin/ y comenzará a monitorearlo automáticamente.

## Funcionamiento
1. El Scanner recorre recursivamente el directorio.
2. El Monitor compara periódicamente los datos.
3. Cuando detecta cambios:
   - Archivos nuevos/modificados
   - Archivos eliminados
4. El Monitor asigna los archivos a los Workers utilizando Pipes.
5. Los Workers copian los archivos al directorio `backup/`.
6. Los Workers actualizan las estadísticas compartidas.
7. El Logger registra los eventos del sistema.

## Estadísticas
Durante la ejecución se mantienen estadísticas compartidas:

- Archivos copiados
- Bytes copiados
- Errores de copia
- Ultimo archivo copiado

Estas estadísticas son protegidas mediante semáforos para evitar condiciones de carrera.

## Tecnologías utilizadas
- Lenguaje C
- POSIX API
- Pipes
- Fork
- Memoria Compartida (`shm_open`, `mmap`)
- Semáforos POSIX
- Cola de mensajes POSIX
- Sistema de archivos Linux

## Autores
- Sebastian Molina