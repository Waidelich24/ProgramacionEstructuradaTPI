#include <stdio.h>

#include "archivos.h"
#include "utiles.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "consultas.h"

int main(void)
{
    int opcion;

    /* Se verifica que los archivos puedan abrirse antes de operar */
    if (!inicializarArchivos()) {
        printf("No se puede iniciar el sistema: revise los permisos de "
               "la carpeta '%s'.\n", DIRECTORIO_DATOS);
        return 1;
    }

    do {
        printf("\n========================================\n");
        printf("   SISTEMA DE ALQUILER DE VEHICULOS\n");
        printf("========================================\n");
        printf("1. Clientes\n");
        printf("2. Vehiculos\n");
        printf("3. Alquileres\n");
        printf("4. Devoluciones\n");
        printf("5. Pagos\n");
        printf("6. Consultas y Reportes\n");
        printf("0. Salir\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1: menuClientes();        break;
            case 2: menuVehiculos();       break;
            case 3: menuAlquileres();      break;
            case 4: registrarDevolucion(); break;
            case 5: menuMovimientos();     break;
            case 6: menuConsultas();       break;
            case 0: printf("Cerrando sistema...\n"); break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);

    return 0;
}
