
#include <stdio.h>
#include <string.h>

#include "vehiculos.h"
#include "archivos.h"

#define ARCHIVO_VEHICULOS "datos/vehiculos.dat"
#define MAX_VEHICULOS 500

int buscarVehiculoPorPatente(const char patente[]) {
    Vehiculo vehiculos[MAX_VEHICULOS];

    int cantidad = leerRegistros(
        ARCHIVO_VEHICULOS,
        vehiculos,
        sizeof(Vehiculo),
        MAX_VEHICULOS
    );

    if (cantidad < 0) {
        return -1;
    }

    int i;

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, patente) == 0) {
            return i;
        }
    }

    return -1;
}

void altaVehiculo(void) {
   
    Vehiculo nuevo;
    FILE *archivo;

    printf("\n--- Alta de vehiculo ---\n");

    printf("Patente: ");
    scanf(" %11[^\n]", nuevo.patente);

    if (buscarVehiculoPorPatente(nuevo.patente) != -1) {
        printf("Error: ya existe un vehiculo con esa patente.\n");
        return;
    }

    printf("Marca: ");
    scanf(" %39[^\n]", nuevo.marca);

    printf("Modelo: ");
    scanf(" %39[^\n]", nuevo.modelo);

    printf("Anio: ");
    scanf("%d", &nuevo.anio);

    printf("Tipo: ");
    scanf(" %29[^\n]", nuevo.tipo);

    printf("Capacidad: ");
    scanf("%d", &nuevo.capacidad);

    printf("Precio por dia: ");
    scanf("%f", &nuevo.precioDia);

    nuevo.estado = VEHICULO_DISPONIBLE;

    archivo = fopen(ARCHIVO_VEHICULOS, "ab");

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
        return;
    }

    if (fwrite(&nuevo, sizeof(Vehiculo), 1, archivo) != 1) {
        printf("Error al guardar el vehiculo.\n");
        fclose(archivo);
        return;
    }

    fclose(archivo);

    printf("Vehiculo registrado correctamente.\n");
}

void listarVehiculos(void) {
    printf("\n--- Lista de vehiculos ---\n");
}

void consultarVehiculo(void) {
    printf("\n--- Consultar vehiculo ---\n");
}

void modificarVehiculo(void) {
    printf("\n--- Modificar vehiculo ---\n");
}

void listarVehiculosDisponibles(void) {
    printf("\n--- Vehiculos disponibles ---\n");
}
