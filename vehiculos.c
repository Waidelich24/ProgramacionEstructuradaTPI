
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
    Vehiculo vehiculos[MAX_VEHICULOS];
    FILE *archivo;
    int cantidad;
    int i;

    printf("\n--- Alta de vehiculo ---\n");

    printf("Ingrese la patente: ");
    scanf(" %11s", nuevo.patente);

    cantidad = leerRegistros(
        ARCHIVO_VEHICULOS,
        vehiculos,
        sizeof(Vehiculo),
        MAX_VEHICULOS
    );

    /*
       Si el archivo no existe, puede ser el primer alta.
       Si existe pero no se puede leer, hay que revisar el error.
    */
    if (cantidad < 0) {
        archivo = fopen(ARCHIVO_VEHICULOS, "rb");

        if (archivo != NULL) {
            fclose(archivo);
            printf("Error al leer el archivo de vehiculos.\n");
            return;
        }

        cantidad = 0;
    }

    if (cantidad >= MAX_VEHICULOS) {
        printf("No se pueden registrar mas vehiculos.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, nuevo.patente) == 0) {
            printf("Ya existe un vehiculo con esa patente.\n");
            return;
        }
    }

    printf("Ingrese la marca: ");
    scanf(" %39[^\n]", nuevo.marca);

    printf("Ingrese el modelo: ");
    scanf(" %39[^\n]", nuevo.modelo);

    printf("Ingrese el anio: ");
    if (scanf("%d", &nuevo.anio) != 1) {
        printf("Anio invalido.\n");
        return;
    }

    printf("Ingrese el tipo de vehiculo: ");
    scanf(" %29[^\n]", nuevo.tipo);

    printf("Ingrese la capacidad: ");
    if (scanf("%d", &nuevo.capacidad) != 1) {
        printf("Capacidad invalida.\n");
        return;
    }

    printf("Ingrese el precio por dia: ");
    if (scanf("%f", &nuevo.precioDia) != 1) {
        printf("Precio invalido.\n");
        return;
    }

    nuevo.estado = VEHICULO_DISPONIBLE;

    archivo = fopen(ARCHIVO_VEHICULOS, "ab");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de vehiculos.\n");
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
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;
    int i;

    printf("\n--- Lista de vehiculos ---\n");

    cantidad = leerRegistros(
        ARCHIVO_VEHICULOS,
        vehiculos,
        sizeof(Vehiculo),
        MAX_VEHICULOS
    );

    if (cantidad < 0) {
        printf("Error al leer el archivo de vehiculos.\n");
        return;
    }

    if (cantidad == 0) {
        printf("No hay vehiculos registrados.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        printf("\n--- Vehiculo %d ---\n", i + 1);
        printf("Patente: %s\n", vehiculos[i].patente);
        printf("Marca: %s\n", vehiculos[i].marca);
        printf("Modelo: %s\n", vehiculos[i].modelo);
        printf("Anio: %d\n", vehiculos[i].anio);
        printf("Tipo: %s\n", vehiculos[i].tipo);
        printf("Capacidad: %d\n", vehiculos[i].capacidad);
        printf("Precio por dia: $%.2f\n", vehiculos[i].precioDia);

        switch (vehiculos[i].estado) {
            case VEHICULO_DISPONIBLE:
                printf("Estado: Disponible\n");
                break;

            case VEHICULO_ALQUILADO:
                printf("Estado: Alquilado\n");
                break;

            case VEHICULO_MANTENIMIENTO:
                printf("Estado: En mantenimiento\n");
                break;

            default:
                printf("Estado: Desconocido\n");
                break;
        }
    }
}

void consultarVehiculo(void) {
    Vehiculo vehiculos[MAX_VEHICULOS];
    char patente[12];
    int cantidad;
    int i;
    int encontrado = 0;

    printf("\n--- Consultar vehiculo ---\n");

    printf("Ingrese la patente: ");
    scanf(" %11s", patente);

    cantidad = leerRegistros(
        ARCHIVO_VEHICULOS,
        vehiculos,
        sizeof(Vehiculo),
        MAX_VEHICULOS
    );

    if (cantidad < 0) {
        printf("Error al leer el archivo de vehiculos.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, patente) == 0) {
            encontrado = 1;
            break;
        }
    }

    if (encontrado == 0) {
        printf("No se encontro ningun vehiculo con esa patente.\n");
        return;
    }

    printf("\n--- Datos del vehiculo ---\n");
    printf("Patente: %s\n", vehiculos[i].patente);
    printf("Marca: %s\n", vehiculos[i].marca);
    printf("Modelo: %s\n", vehiculos[i].modelo);
    printf("Anio: %d\n", vehiculos[i].anio);
    printf("Tipo: %s\n", vehiculos[i].tipo);
    printf("Capacidad: %d\n", vehiculos[i].capacidad);
    printf("Precio por dia: $%.2f\n", vehiculos[i].precioDia);

    switch (vehiculos[i].estado) {
        case VEHICULO_DISPONIBLE:
            printf("Estado: Disponible\n");
            break;

        case VEHICULO_ALQUILADO:
            printf("Estado: Alquilado\n");
            break;

        case VEHICULO_MANTENIMIENTO:
            printf("Estado: En mantenimiento\n");
            break;

        default:
            printf("Estado: Desconocido\n");
            break;
    }
}

void modificarVehiculo(void) {
    Vehiculo vehiculos[MAX_VEHICULOS];
    char patente[12];
    int cantidad;
    int i;
    int encontrado = 0;
    FILE *archivo;

    printf("\n--- Modificar vehiculo ---\n");

    printf("Ingrese la patente del vehiculo: ");
    scanf(" %11s", patente);

    cantidad = leerRegistros(
        ARCHIVO_VEHICULOS,
        vehiculos,
        sizeof(Vehiculo),
        MAX_VEHICULOS
    );

    if (cantidad < 0) {
        printf("Error al leer el archivo de vehiculos.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, patente) == 0) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("No se encontro ningun vehiculo con esa patente.\n");
        return;
    }

    printf("\nVehiculo encontrado: %s %s\n",
           vehiculos[i].marca, vehiculos[i].modelo);

    printf("Ingrese la nueva marca: ");
    scanf(" %39[^\n]", vehiculos[i].marca);

    printf("Ingrese el nuevo modelo: ");
    scanf(" %39[^\n]", vehiculos[i].modelo);

    printf("Ingrese el nuevo anio: ");
    if (scanf("%d", &vehiculos[i].anio) != 1) {
        printf("Anio invalido.\n");
        return;
    }

    printf("Ingrese el nuevo tipo de vehiculo: ");
    scanf(" %29[^\n]", vehiculos[i].tipo);

    printf("Ingrese la nueva capacidad: ");
    if (scanf("%d", &vehiculos[i].capacidad) != 1) {
        printf("Capacidad invalida.\n");
        return;
    }

    printf("Ingrese el nuevo precio por dia: ");
    if (scanf("%f", &vehiculos[i].precioDia) != 1) {
        printf("Precio invalido.\n");
        return;
    }

    archivo = fopen(ARCHIVO_VEHICULOS, "wb");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo para guardar los cambios.\n");
        return;
    }

    if (fwrite(vehiculos, sizeof(Vehiculo), cantidad, archivo)
        != (size_t)cantidad) {
        printf("Error al guardar los cambios.\n");
        fclose(archivo);
        return;
    }

    fclose(archivo);

    printf("Vehiculo modificado correctamente.\n");
}

void listarVehiculosDisponibles(void) {
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;
    int i;
    int encontrados = 0;

    printf("\n--- Vehiculos disponibles ---\n");

    cantidad = leerRegistros(
        ARCHIVO_VEHICULOS,
        vehiculos,
        sizeof(Vehiculo),
        MAX_VEHICULOS
    );

    if (cantidad < 0) {
        printf("Error al leer el archivo de vehiculos.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (vehiculos[i].estado == VEHICULO_DISPONIBLE) {
            encontrados++;

            printf("\n--- Vehiculo %d ---\n", encontrados);
            printf("Patente: %s\n", vehiculos[i].patente);
            printf("Marca: %s\n", vehiculos[i].marca);
            printf("Modelo: %s\n", vehiculos[i].modelo);
            printf("Anio: %d\n", vehiculos[i].anio);
            printf("Tipo: %s\n", vehiculos[i].tipo);
            printf("Capacidad: %d\n", vehiculos[i].capacidad);
            printf("Precio por dia: $%.2f\n",
                   vehiculos[i].precioDia);
        }
    }

    if (encontrados == 0) {
        printf("No hay vehiculos disponibles.\n");
    }
}
