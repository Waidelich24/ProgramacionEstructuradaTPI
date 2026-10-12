
#include <stdio.h>

#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "consultas.h"

int main(void) {
    int opcion;

    do {
        printf("\n================================\n");
        printf(" SISTEMA DE ALQUILER DE VEHICULOS\n");
        printf("================================\n");

        printf("1. Registrar cliente\n");
        printf("2. Registrar vehiculo\n");
        printf("3. Realizar alquiler\n");
        printf("4. Registrar devolucion\n");
        printf("5. Registrar pago\n");
        printf("6. Consultar cliente\n");
        printf("7. Listar alquileres\n");
        printf("8. Consultas y reportes\n");
        printf("9. Listar clientes\n");
        printf("10. Listar vehiculos\n");
        printf("0. Salir\n");

        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n');
            opcion = -1;
        }

        switch (opcion) {
            case 1:
                altaCliente();
                break;

            case 2:
                altaVehiculo();
                break;

            case 3:
                realizarAlquiler();
                break;

            case 4:
                registrarDevolucion();
                break;

            case 5:
                registrarPago();
                break;

            case 6:
                consultarCliente();
                break;

            case 7:
                listarAlquileres();
                break;

            case 8:
                menuConsultas();
                break;

            case 9:
                listarClientes();
                break;

            case 10:
                listarVehiculos();
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