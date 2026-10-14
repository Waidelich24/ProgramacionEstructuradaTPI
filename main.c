#include <stdio.h>

#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "consultas.h"

static void limpiarEntrada(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
        /* Limpiar entrada */
    }
}


int main(void)
{
    int opcion;

    do {
        printf("\n================================\n");
        printf(" SISTEMA DE ALQUILER DE VEHICULOS\n");
        printf("================================\n");
        printf("1. Alquileres\n");
        printf("2. Vehiculos\n");
        printf("3. Clientes\n");
        printf("4. Movimientos\n");
        printf("5. Reportes\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarEntrada();
            printf("Opcion invalida.\n");
            opcion = -1;
            continue;
        }

        limpiarEntrada();

        switch (opcion) {
            case 1:
                menuAlquileres();
                break;

            case 2:
                altaVehiculo();
                listarVehiculos();
                break;

            case 3:
                altaCliente();
                listarClientes();
                consultarCliente();
                break;

            case 4:
                menuMovimientos(); // <--- Llamada al submenú de movimientos
                break;

            case 5:
                menuConsultas();
                break;

            case 0:
                printf("Cerrando sistema...\n");
                break;

            default:
                printf("Opcion invalida.\n");
        }

    } while (opcion != 0);

    return 0;
}
