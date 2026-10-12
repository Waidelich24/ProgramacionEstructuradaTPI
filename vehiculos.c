
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
    printf("\n--- Alta de vehiculo ---\n");
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
