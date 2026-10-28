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
        
    }
}

int main(void)
{
    int opcion;

    do {
        printf("\n==================================================\n");
        printf("         SISTEMA DE ALQUILER DE VEHICULOS         \n");
        printf("==================================================\n");
        printf("1. Clientes\n");
        printf("   > a. Dar de alta cliente\n");
        printf("   > b. Modificar cliente\n");
        printf("   > c. Consultar cliente\n");
        printf("--------------------------------------------------\n");
        printf("2. Vehiculos\n");
        printf("   > a. Dar de alta vehiculo\n");
        printf("   > b. Modificar vehiculo\n");
        printf("   > c. Buscar vehiculo\n");
        printf("--------------------------------------------------\n");
        printf("3. Alquileres\n");
        printf("   > a. Realizar alquiler\n");
        printf("   > b. Registrar devolucion\n");
        printf("   > c. Listar alquileres\n");
        printf("--------------------------------------------------\n");
        printf("4. Movimientos / Pagos\n");
        printf("   > a. Registrar pago\n");
        printf("   > b. Consultar movimientos\n");
        printf("--------------------------------------------------\n");
        printf("5. Consultas y Reportes\n");
        printf("0. Salir\n");
        printf("==================================================\n");
        printf("Seleccione una opcion del menu principal: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarEntrada();
            printf("Opcion invalida.\n");
            opcion = -1;
            continue;
        }

        limpiarEntrada();

        switch (opcion) {
            case 1:
				menuClientes();
                break;
			case 2:
			    menuVehiculos();
  				  break;

            case 3:
                menuAlquileres(); // Tu submenú de alquileres
                break;

            case 4:
menuMovimientos();
                break;

            case 5:
                menuConsultas(); // Reportes
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
