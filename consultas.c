#include <stdio.h>
#include <string.h>

#include "consultas.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

/* =========================================================
   FUNCIONES AUXILIARES
   ========================================================= */

static int cargarAlquileres(Alquiler alquileres[], int *cantidad)
{
    int resultado = leerRegistros(ARCHIVO_ALQUILERES, alquileres,
                                  sizeof(Alquiler), MAX_ALQUILERES);

    if (resultado < 0) {
        printf("Error: no se pudo leer el archivo de alquileres.\n");
        return 0;
    }

    *cantidad = resultado;
    return 1;
}

/* Pide un rango de fechas y verifica que el inicial no sea posterior */
static int pedirRangoFechas(Fecha *desde, Fecha *hasta)
{
    if (!pedirFecha("Fecha inicial del periodo", desde)) {
        return 0;
    }

    while (1) {
        if (!pedirFecha("Fecha final del periodo", hasta)) {
            return 0;
        }

        if (compararFechas(*desde, *hasta) <= 0) {
            return 1;
        }

        printf("Error: la fecha final no puede ser anterior a la inicial.\n");
    }
}

static int fechaEnRango(Fecha fecha, Fecha desde, Fecha hasta)
{
    return compararFechas(fecha, desde) >= 0 &&
           compararFechas(fecha, hasta) <= 0;
}

static void imprimirPeriodo(Fecha desde, Fecha hasta)
{
    printf("Periodo: ");
    imprimirFecha(desde);
    printf(" al ");
    imprimirFecha(hasta);
    printf("\n\n");
}

/* =========================================================
   MENU
   ========================================================= */

void menuConsultas(void)
{
    int opcion;

    do {
        printf("\n========================================\n");
        printf("         CONSULTAS Y REPORTES\n");
        printf("========================================\n");
        printf(" 1. Vehiculos disponibles\n");
        printf(" 2. Vehiculos alquilados\n");
        printf(" 3. Alquileres por rango de fechas\n");
        printf(" 4. Alquileres por cliente\n");
        printf(" 5. Alquileres por vehiculo\n");
        printf(" 6. Alquileres por estado\n");
        printf(" 7. Alquileres por importe\n");
        printf(" 8. Pagos realizados en un periodo\n");
        printf(" 9. Clientes con deuda\n");
        printf("10. Estadisticas de la empresa\n");
        printf(" 0. Volver al menu principal\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1:  vehiculosDisponibles();   break;
            case 2:  vehiculosAlquilados();    break;
            case 3:  alquileresPorFecha();     break;
            case 4:  alquileresPorCliente();   break;
            case 5:  alquileresPorVehiculo();  break;
            case 6:  alquileresPorEstado();    break;
            case 7:  alquileresPorImporte();   break;
            case 8:  pagosPorPeriodo();        break;
            case 9:  clientesConDeuda();       break;
            case 10: estadisticasEmpresa();    break;
            case 0:                            break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}

/* =========================================================
   VEHICULOS
   ========================================================= */

void vehiculosDisponibles(void)
{
    listarVehiculosDisponibles();
}

void vehiculosAlquilados(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Cliente cliente;
    Vehiculo vehiculo;
    char marcaModelo[90];
    char clienteTexto[100];
    int cantidad;
    int i;
    int resultado;
    int encontrados = 0;

    mostrarTitulo("VEHICULOS ALQUILADOS");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].estado != ALQUILER_ACTIVO) {
            continue;
        }

        resultado = obtenerVehiculo(alquileres[i].patente, &vehiculo);

        if (resultado < 0) {
            return;
        }

        if (resultado == 1) {
            snprintf(marcaModelo, sizeof(marcaModelo), "%s %s",
                     vehiculo.marca, vehiculo.modelo);
        } else {
            snprintf(marcaModelo, sizeof(marcaModelo),
                     "(vehiculo no encontrado)");
        }

        resultado = obtenerCliente(alquileres[i].dniCliente, &cliente);

        if (resultado < 0) {
            return;
        }

        if (resultado == 1) {
            snprintf(clienteTexto, sizeof(clienteTexto), "%s (%s)",
                     cliente.nombre, cliente.dni);
        } else {
            snprintf(clienteTexto, sizeof(clienteTexto), "%s (no encontrado)",
                     alquileres[i].dniCliente);
        }

        if (encontrados == 0) {
            printf("%-9s %-26s %-40s %-10s %-10s\n", "Patente",
                   "Marca y modelo", "Cliente", "Inicio", "Prevista");
            printf("-----------------------------------------------------"
                   "-----------------------------------------------\n");
        }

        printf("%-9s %-26.26s %-40.40s ", alquileres[i].patente,
               marcaModelo, clienteTexto);
        imprimirFecha(alquileres[i].fechaInicio);
        printf(" ");
        imprimirFecha(alquileres[i].fechaPrevista);
        printf("\n");

        encontrados++;
    }

    if (encontrados == 0) {
        printf("No hay vehiculos alquilados actualmente.\n");
    } else {
        printf("\nVehiculos alquilados: %d\n", encontrados);
    }
}

/* =========================================================
   ALQUILERES
   ========================================================= */

void alquileresPorFecha(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Fecha desde;
    Fecha hasta;
    int cantidad;
    int i;
    int encontrados = 0;

    mostrarTitulo("ALQUILERES POR RANGO DE FECHAS");

    if (!pedirRangoFechas(&desde, &hasta)) {
        mostrarCancelado();
        return;
    }

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    printf("\n");
    imprimirPeriodo(desde, hasta);

    for (i = 0; i < cantidad; i++) {
        if (fechaEnRango(alquileres[i].fechaInicio, desde, hasta)) {
            if (encontrados == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No hay alquileres iniciados dentro del periodo indicado.\n");
    } else {
        printf("\nAlquileres encontrados: %d\n", encontrados);
    }
}

void alquileresPorCliente(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Cliente cliente;
    char dni[12];
    int cantidad;
    int resultado;
    int i;
    int encontrados = 0;

    mostrarTitulo("ALQUILERES POR CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        return;
    }

    resultado = obtenerCliente(dni, &cliente);

    if (resultado < 0) {
        return;
    }

    if (resultado == 0) {
        printf("Error: no existe un cliente con el DNI %s.\n", dni);
        return;
    }

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    printf("\nCliente: %s (DNI %s)\n\n", cliente.nombre, cliente.dni);

    for (i = 0; i < cantidad; i++) {
        if (strcmp(alquileres[i].dniCliente, dni) == 0) {
            if (encontrados == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("El cliente no tiene alquileres registrados.\n");
    } else {
        printf("\nAlquileres encontrados: %d\n", encontrados);
    }
}

void alquileresPorVehiculo(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Vehiculo vehiculo;
    char patente[12];
    int cantidad;
    int resultado;
    int i;
    int encontrados = 0;

    mostrarTitulo("ALQUILERES POR VEHICULO");

    if (!pedirPatente(patente)) {
        mostrarCancelado();
        return;
    }

    resultado = obtenerVehiculo(patente, &vehiculo);

    if (resultado < 0) {
        return;
    }

    if (resultado == 0) {
        printf("Error: no existe un vehiculo con la patente %s.\n", patente);
        return;
    }

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    printf("\nVehiculo: %s %s (%s)\n\n", vehiculo.marca, vehiculo.modelo,
           vehiculo.patente);

    for (i = 0; i < cantidad; i++) {
        if (strcmp(alquileres[i].patente, patente) == 0) {
            if (encontrados == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("El vehiculo no tiene alquileres registrados.\n");
    } else {
        printf("\nAlquileres encontrados: %d\n", encontrados);
    }
}

void alquileresPorEstado(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    EstadoAlquiler estado;
    int cantidad;
    int opcion;
    int i;
    int encontrados = 0;

    mostrarTitulo("ALQUILERES POR ESTADO");
    printf("1. Activos\n");
    printf("2. Finalizados\n");
    printf("3. Cancelados\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione el estado: ");

    if (opcion == 0) {
        return;
    }

    if (opcion < 1 || opcion > 3) {
        printf("Error: opcion invalida.\n");
        return;
    }

    estado = (EstadoAlquiler)(opcion - 1);

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    printf("\nAlquileres %s:\n\n", textoEstadoAlquiler(estado));

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].estado == estado) {
            if (encontrados == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No hay alquileres con ese estado.\n");
    } else {
        printf("\nAlquileres encontrados: %d\n", encontrados);
    }
}

void alquileresPorImporte(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    float importe;
    int cantidad;
    int opcion;
    int i;
    int coincide;
    int encontrados = 0;

    mostrarTitulo("ALQUILERES POR IMPORTE");

    if (!pedirMonto("Importe de referencia", 0.0f, &importe)) {
        mostrarCancelado();
        return;
    }

    printf("\n1. Alquileres con importe SUPERIOR a $%.2f\n", importe);
    printf("2. Alquileres con importe INFERIOR a $%.2f\n", importe);
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione una opcion: ");

    if (opcion == 0) {
        return;
    }

    if (opcion != 1 && opcion != 2) {
        printf("Error: opcion invalida.\n");
        return;
    }

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    printf("\n");

    for (i = 0; i < cantidad; i++) {
        coincide = (opcion == 1) ? alquileres[i].importeTotal > importe
                                 : alquileres[i].importeTotal < importe;

        if (coincide) {
            if (encontrados == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No hay alquileres que cumplan esa condicion.\n");
    } else {
        printf("\nAlquileres encontrados: %d\n", encontrados);
    }
}

/* =========================================================
   PAGOS Y DEUDAS
   ========================================================= */

void pagosPorPeriodo(void)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    Fecha desde;
    Fecha hasta;
    double total = 0.0;
    int cantidad;
    int i;
    int encontrados = 0;

    mostrarTitulo("PAGOS REALIZADOS EN UN PERIODO");

    if (!pedirRangoFechas(&desde, &hasta)) {
        mostrarCancelado();
        return;
    }

    cantidad = leerRegistros(ARCHIVO_MOVIMIENTOS, movimientos,
                             sizeof(Movimiento), MAX_MOVIMIENTOS);

    if (cantidad < 0) {
        printf("Error: no se pudo leer el archivo de movimientos.\n");
        return;
    }

    printf("\n");
    imprimirPeriodo(desde, hasta);

    for (i = 0; i < cantidad; i++) {
        if (movimientos[i].tipo == MOV_PAGO &&
            fechaEnRango(movimientos[i].fecha, desde, hasta)) {
            if (encontrados == 0) {
                imprimirEncabezadoMovimientos();
            }

            imprimirFilaMovimiento(&movimientos[i]);
            total += movimientos[i].monto;
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No hay pagos registrados en el periodo indicado.\n");
    } else {
        printf("\nPagos encontrados: %d - Total cobrado: $%.2f\n",
               encontrados, total);
    }
}

void clientesConDeuda(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Cliente cliente;
    char nombre[80];
    double totalDeuda = 0.0;
    float saldo;
    int cantidad;
    int resultado;
    int i;
    int encontrados = 0;

    mostrarTitulo("CLIENTES CON DEUDA");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    for (i = 0; i < cantidad; i++) {
        saldo = saldoAlquiler(&alquileres[i]);

        if (saldo <= 0) {
            continue;
        }

        resultado = obtenerCliente(alquileres[i].dniCliente, &cliente);

        if (resultado < 0) {
            return;
        }

        if (resultado == 1) {
            snprintf(nombre, sizeof(nombre), "%s", cliente.nombre);
        } else {
            snprintf(nombre, sizeof(nombre), "(cliente no encontrado)");
        }

        if (encontrados == 0) {
            printf("%-9s %-28s %-6s %12s %12s %12s\n", "DNI", "Nombre",
                   "Alq.", "Total", "Pagado", "Saldo");
            printf("-----------------------------------------------------"
                   "-----------------------------\n");
        }

        printf("%-9s %-28.28s %-6d %12.2f %12.2f %12.2f\n",
               alquileres[i].dniCliente, nombre, alquileres[i].numero,
               alquileres[i].importeTotal, alquileres[i].importePagado,
               saldo);

        totalDeuda += saldo;
        encontrados++;
    }

    if (encontrados == 0) {
        printf("No hay clientes con deuda.\n");
    } else {
        printf("\nAlquileres con saldo pendiente: %d - Deuda total: $%.2f\n",
               encontrados, totalDeuda);
    }
}

/* =========================================================
   ESTADISTICAS
   ========================================================= */

void estadisticasEmpresa(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Vehiculo vehiculos[MAX_VEHICULOS];
    double cobrado = 0.0;
    double pendiente = 0.0;
    int cantAlquileres;
    int cantVehiculos;
    int disponibles = 0;
    int alquilados = 0;
    int mantenimiento = 0;
    int baja = 0;
    int activos = 0;
    int i;

    mostrarTitulo("ESTADISTICAS DE LA EMPRESA");

    cantVehiculos = leerRegistros(ARCHIVO_VEHICULOS, vehiculos,
                                  sizeof(Vehiculo), MAX_VEHICULOS);

    if (cantVehiculos < 0) {
        printf("Error: no se pudo leer el archivo de vehiculos.\n");
        return;
    }

    if (!cargarAlquileres(alquileres, &cantAlquileres)) {
        return;
    }

    for (i = 0; i < cantVehiculos; i++) {
        switch (vehiculos[i].estado) {
            case VEHICULO_DISPONIBLE:    disponibles++;    break;
            case VEHICULO_ALQUILADO:     alquilados++;     break;
            case VEHICULO_MANTENIMIENTO: mantenimiento++;  break;
            case VEHICULO_BAJA:          baja++;           break;
            default:                                       break;
        }
    }

    for (i = 0; i < cantAlquileres; i++) {
        cobrado += alquileres[i].importePagado;
        pendiente += saldoAlquiler(&alquileres[i]);

        if (alquileres[i].estado == ALQUILER_ACTIVO) {
            activos++;
        }
    }

    printf("Cantidad total de vehiculos:        %d\n", cantVehiculos);
    printf("  Vehiculos disponibles:            %d\n", disponibles);
    printf("  Vehiculos alquilados:             %d\n", alquilados);
    printf("  Vehiculos en mantenimiento:       %d\n", mantenimiento);
    printf("  Vehiculos dados de baja:          %d\n", baja);
    printf("Cantidad total de alquileres:       %d\n", cantAlquileres);
    printf("  Alquileres activos:               %d\n", activos);
    printf("Importe total cobrado:              $%.2f\n", cobrado);
    printf("Importe total pendiente de cobro:   $%.2f\n", pendiente);
}
