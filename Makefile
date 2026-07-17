# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Wno-unused-parameter -Wno-unused-variable -Wno-format-truncation
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

# Mostrar ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make clean    - Eliminar archivos compilados"
	@echo "  make clean-all - Eliminar todo (incluye archivos de prueba)"
	@echo "  make run      - Ejecutar el programa"
	@echo "  make help     - Mostrar esta ayuda"

.PHONY: all clean clean-all run rund setup test help