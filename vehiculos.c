
#include <stdio.h>
#include <string.h>

#include "vehiculos.h"
#include "archivos.h"

#define ARCHIVO_VEHICULOS "datos/vehiculos.dat"
#define MAX_VEHICULOS 500

void menuVehiculos(void)
{
    int opcion;
    int c;

    do {
        printf("\n========================================\n");
        printf("          MENU DE VEHICULOS             \n");
        printf("========================================\n");
        printf("1. Dar de alta vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Consultar vehiculo por patente\n");
        printf("4. Listar todos los vehiculos\n");
        printf("5. Listar vehiculos disponibles\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            while ((c = getchar()) != '\n' && c != EOF) {
            }

            printf("Opcion invalida.\n");
            opcion = -1;
            continue;
        }

        while ((c = getchar()) != '\n' && c != EOF) {
        }

        switch (opcion) {
            case 1:
                altaVehiculo();
                break;

            case 2:
                modificarVehiculo();
                break;

            case 3:
                consultarVehiculo();
                break;

            case 4:
                listarVehiculos();
                break;

            case 5:
                listarVehiculosDisponibles();
                break;

            case 0:
                printf("Volviendo al menu principal...\n");
                break;

            default:
                printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}

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


void modificarVehiculo(void)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    char patente[12];
    int cantidad;
    int i;
    int encontrado = 0;
    int opcion;
    int estado;
    int c;
    FILE *archivo;

    printf("\n--- Modificar vehiculo ---\n");
    printf("Ingrese la patente del vehiculo: ");
    scanf(" %11s", patente);

    /* Leer los vehiculos registrados */
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

    /* Buscar el vehiculo por patente */
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

    /* Mostrar los datos actuales del vehiculo */
    printf("\n--- Datos del vehiculo a modificar ---\n");
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

    /* Menu de opciones */
    printf("\nQue desea modificar?\n");
    printf("1. Marca\n");
    printf("2. Modelo\n");
    printf("3. Anio\n");
    printf("4. Tipo de vehiculo\n");
    printf("5. Capacidad\n");
    printf("6. Precio por dia\n");
    printf("7. Estado\n");
    printf("8. Modificar todos los datos\n");
    printf("0. Cancelar\n");
    printf("Seleccione una opcion: ");

    if (scanf("%d", &opcion) != 1) {
        while ((c = getchar()) != '\n' && c != EOF) {
        }
        printf("Opcion invalida.\n");
        return;
    }

    while ((c = getchar()) != '\n' && c != EOF) {
    }

    /* Modificar el campo seleccionado */
    switch (opcion) {
        case 1:
            printf("Ingrese la nueva marca: ");
            scanf(" %39[^\n]", vehiculos[i].marca);
            break;

        case 2:
            printf("Ingrese el nuevo modelo: ");
            scanf(" %39[^\n]", vehiculos[i].modelo);
            break;

        case 3:
            printf("Ingrese el nuevo anio: ");
            if (scanf("%d", &vehiculos[i].anio) != 1) {
                printf("Anio invalido.\n");
                return;
            }
            break;

        case 4:
            printf("Ingrese el nuevo tipo de vehiculo: ");
            scanf(" %29[^\n]", vehiculos[i].tipo);
            break;

        case 5:
            printf("Ingrese la nueva capacidad: ");
            if (scanf("%d", &vehiculos[i].capacidad) != 1) {
                printf("Capacidad invalida.\n");
                return;
            }
            break;

        case 6:
            printf("Ingrese el nuevo precio por dia: ");
            if (scanf("%f", &vehiculos[i].precioDia) != 1) {
                printf("Precio invalido.\n");
                return;
            }
            break;

        case 7:
            printf("\nSeleccione el nuevo estado:\n");
            printf("1. Disponible\n");
            printf("2. Alquilado\n");
            printf("3. En mantenimiento\n");
            printf("Opcion: ");

            if (scanf("%d", &estado) != 1) {
                printf("Estado invalido.\n");
                return;
            }

            switch (estado) {
                case 1:
                    vehiculos[i].estado = VEHICULO_DISPONIBLE;
                    break;

                case 2:
                    vehiculos[i].estado = VEHICULO_ALQUILADO;
                    break;

                case 3:
                    vehiculos[i].estado = VEHICULO_MANTENIMIENTO;
                    break;

                default:
                    printf("Estado invalido.\n");
                    return;
            }
            break;

        case 8:
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

            printf("\nSeleccione el nuevo estado:\n");
            printf("1. Disponible\n");
            printf("2. Alquilado\n");
            printf("3. En mantenimiento\n");
            printf("Opcion: ");

            if (scanf("%d", &estado) != 1) {
                printf("Estado invalido.\n");
                return;
            }

            switch (estado) {
                case 1:
                    vehiculos[i].estado = VEHICULO_DISPONIBLE;
                    break;

                case 2:
                    vehiculos[i].estado = VEHICULO_ALQUILADO;
                    break;

                case 3:
                    vehiculos[i].estado = VEHICULO_MANTENIMIENTO;
                    break;

                default:
                    printf("Estado invalido.\n");
                    return;
            }
            break;

        case 0:
            printf("Modificacion cancelada.\n");
            return;

        default:
            printf("Opcion invalida.\n");
            return;
    }

    /* Guardar los cambios en el archivo */
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

    printf("\nVehiculo modificado correctamente.\n");

    /* Mostrar los datos actualizados */
    printf("\n--- Datos actualizados ---\n");
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
