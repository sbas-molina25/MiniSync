# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lrt -pthread

# Directorios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Archivos fuente
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/minisync

# Regla principal
all: $(TARGET)

# Crear ejecutable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilar archivos .c a .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Limpiar y eliminar también backup y archivos de prueba
clean-all: clean
	rm -rf ~/minisync_test

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET) test

# Ejecutar en modo daemon
rund: $(TARGET)
	./$(TARGET) ~/minisync_test/origen &

# Crear entorno de pruebas
setup:
	@mkdir -p test/origen
	@mkdir -p test/backup
	@cd test/origen && \
		echo "Hola mundo" > archivo1.txt && \
		dd if=/dev/zero of=archivo2.bin bs=1M count=10 2>/dev/null && \
		echo "Archivo con permisos" > archivo3.txt && \
		chmod 755 archivo3.txt && \
		mkdir -p subdir1/subdir2 && \
		echo "Archivo en subdirectorio" > subdir1/archivo4.txt && \
		echo "Archivo profundo" > subdir1/subdir2/archivo5.txt
	@echo "Entorno de pruebas creado en ~/minisync_test"

# Ejecutar pruebas
test: $(TARGET) setup
	@echo "=== EJECUTANDO PRUEBAS ==="
	@./$(TARGET) ~/minisync_test/origen &
	@sleep 10
	@echo "Verificando backup..."
	@ls -la ~/minisync_test/backup/
	@echo "=== PRUEBAS COMPLETADAS ==="
	@pkill minisync || true

# Mostrar ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make clean    - Eliminar archivos compilados"
	@echo "  make clean-all - Eliminar todo (incluye archivos de prueba)"
	@echo "  make run      - Ejecutar el programa"
	@echo "  make rund     - Ejecutar como daemon"
	@echo "  make setup    - Crear entorno de pruebas"
	@echo "  make test     - Compilar, crear entorno y ejecutar pruebas"
	@echo "  make help     - Mostrar esta ayuda"

.PHONY: all clean clean-all run rund setup test help