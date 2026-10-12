
#include <stdio.h>
#include "consultas.h"

void menuConsultas(void) {
    int opcion;

    do {
        printf("\n===== CONSULTAS =====\n");
        printf("1. Alquileres por fecha\n");
        printf("2. Alquileres por cliente\n");
        printf("3. Alquileres por vehiculo\n");
        printf("4. Alquileres por estado\n");
        printf("5. Alquileres por importe\n");
        printf("6. Pagos por periodo\n");
        printf("7. Clientes con deuda\n");
        printf("8. Estadisticas\n");
        printf("0. Volver\n");
        printf("Opcion: ");

        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n');
            opcion = -1;
        }

        switch (opcion) {
            case 1: alquileresPorFecha(); break;
            case 2: alquileresPorCliente(); break;
            case 3: alquileresPorVehiculo(); break;
            case 4: alquileresPorEstado(); break;
            case 5: alquileresPorImporte(); break;
            case 6: pagosPorPeriodo(); break;
            case 7: clientesConDeuda(); break;
            case 8: estadisticasEmpresa(); break;
            case 0: break;
            default:
                printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}

void alquileresPorFecha(void) {
    printf("Consulta por fecha pendiente.\n");
}

void alquileresPorCliente(void) {
    printf("Consulta por cliente pendiente.\n");
}

void alquileresPorVehiculo(void) {
    printf("Consulta por vehiculo pendiente.\n");
}

void alquileresPorEstado(void) {
    printf("Consulta por estado pendiente.\n");
}

void alquileresPorImporte(void) {
    printf("Consulta por importe pendiente.\n");
}

void pagosPorPeriodo(void) {
    printf("Consulta de pagos pendiente.\n");
}

void clientesConDeuda(void) {
    printf("Consulta de deudas pendiente.\n");
}

void estadisticasEmpresa(void) {
    printf("Estadisticas pendientes.\n");
}