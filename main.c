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
    int opcion = 0;
    int resultado = 0;

    /* se comprueba que los archivos se puedan abrir antes de empezar */
    if (!inicializarArchivos()) {
<<<<<<< HEAD
        printf("No se puede iniciar el sistema.\n");
=======
        printf("No se puede iniciar el sistema: revise los permisos de "
               "la carpeta '%s'.\n", DIRECTORIO_DATOS);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        resultado = 1;
    } else {
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
<<<<<<< HEAD
            opcion = leerOpcion("Seleccione una opcion: ");

            if (opcion == 1) {
                menuClientes();
            } else if (opcion == 2) {
                menuVehiculos();
            } else if (opcion == 3) {
                menuAlquileres();
            } else if (opcion == 4) {
                registrarDevolucion();
            } else if (opcion == 5) {
                menuMovimientos();
            } else if (opcion == 6) {
                menuConsultas();
            } else if (opcion == 0) {
                printf("Cerrando el sistema...\n");
            } else {
                printf("Opcion invalida.\n");
            }
=======
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

>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        } while (opcion != 0);
    }

    return resultado;
}
