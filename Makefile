CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c99

MODULOS = clientes.o vehiculos.o alquileres.o movimientos.o consultas.o \
          archivos.o utiles.o

all: alquiler generar_datos

alquiler: main.o $(MODULOS)
	$(CC) $(CFLAGS) -o $@ $^

# Programa auxiliar que crea los archivos de prueba (carpeta datos/)
generar_datos: generar_datos.o $(MODULOS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o alquiler generar_datos

.PHONY: all clean
