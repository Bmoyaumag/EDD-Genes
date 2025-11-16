# Nombre del ejecutable
OBJETIVO = EDDGenes

# Compilador
CC = gcc

# Flags de compilación (para warnings, normas y compatibilidad)
CFLAGS = -Wall -Wextra -std=c99

# Archivo fuente
FUENTE = $(OBJETIVO).c

# Regla por defecto: compilar el programa
all: $(OBJETIVO)

# Compilar el archivo fuente
$(OBJETIVO): $(FUENTE)
	$(CC) $(CFLAGS) -o $(OBJETIVO) $(FUENTE)

# Limpiar archivos intermedios y ejecutable
clean:
	rm -f $(OBJETIVO) *.o

.PHONY: all clean