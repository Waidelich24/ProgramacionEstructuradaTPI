#include <stdio.h>
#include <string.h>

#include "consultas.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

/* Filtro aplicado a cada alquiler leido del archivo; recibe un dato
   (rango, DNI, patente, estado o importe) segun la consulta. */
typedef int (*FiltroAlquiler)(const Alquiler *alquiler, const void *dato);

typedef struct {
    Fecha desde;
    Fecha hasta;
} Rango;

typedef struct {
    float importe;
    int mayor;      /* 1 = superiores, 0 = inferiores */
} FiltroImporte;

/* =========================================================
   FUNCIONES AUXILIARES
   ========================================================= */

/* Pide un rango de fechas y verifica que el inicial no sea posterior */
static int pedirRangoFechas(Fecha *desde, Fecha *hasta)
{
    int valido = 0;
    int cancelado = !pedirFecha("Fecha inicial del periodo", desde);

    while (!cancelado && !valido) {
        if (!pedirFecha("Fecha final del periodo", hasta)) {
            cancelado = 1;
        } else if (compararFechas(*desde, *hasta) <= 0) {
            valido = 1;
        } else {
            printf("Error: la fecha final no puede ser anterior a la "
                   "inicial.\n");
        }
    }

    return valido;
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

/* Recorre alquileres.dat y muestra los que cumplen el filtro */
static CodigoError listarAlquileresFiltrados(FiltroAlquiler filtro,
                                             const void *dato,
                                             const char *sinResultados)
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    CodigoError error = ERR_OK;
    int encontrados = 0;

    if (archivo == NULL) {
        error = ERR_ARCHIVO_ABRIR;
        mostrarError(error, ARCHIVO_ALQUILERES);
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (filtro(&alquiler, dato)) {
                if (encontrados == 0) {
                    imprimirEncabezadoAlquileres();
                }

                imprimirFilaAlquiler(&alquiler);
                encontrados++;
            }
        }

        cerrarArchivo(archivo);

        if (encontrados == 0) {
            printf("%s\n", sinResultados);
        } else {
            printf("\nAlquileres encontrados: %d\n", encontrados);
        }
    }

    return error;
}

static int filtroPorFecha(const Alquiler *alquiler, const void *dato)
{
    const Rango *rango = (const Rango *)dato;

    return fechaEnRango(alquiler->fechaInicio, rango->desde, rango->hasta);
}

static int filtroPorCliente(const Alquiler *alquiler, const void *dato)
{
    return strcmp(alquiler->dniCliente, (const char *)dato) == 0;
}

static int filtroPorVehiculo(const Alquiler *alquiler, const void *dato)
{
    return strcmp(alquiler->patente, (const char *)dato) == 0;
}

static int filtroPorEstado(const Alquiler *alquiler, const void *dato)
{
    return alquiler->estado == *(const EstadoAlquiler *)dato;
}

static int filtroPorImporte(const Alquiler *alquiler, const void *dato)
{
    const FiltroImporte *filtro = (const FiltroImporte *)dato;

    return filtro->mayor ? alquiler->importeTotal > filtro->importe
                         : alquiler->importeTotal < filtro->importe;
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
    FILE *archivo;
    Alquiler alquiler;
    Cliente cliente;
    Vehiculo vehiculo;
    char marcaModelo[90];
    char clienteTexto[100];
    int encontrados = 0;

    mostrarTitulo("VEHICULOS ALQUILADOS");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_ALQUILERES);
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.estado == ALQUILER_ACTIVO) {
                if (obtenerVehiculo(alquiler.patente, &vehiculo) == ERR_OK) {
                    snprintf(marcaModelo, sizeof(marcaModelo), "%s %s",
                             vehiculo.marca, vehiculo.modelo);
                } else {
                    snprintf(marcaModelo, sizeof(marcaModelo),
                             "(vehiculo no encontrado)");
                }

                if (obtenerCliente(alquiler.dniCliente, &cliente) == ERR_OK) {
                    snprintf(clienteTexto, sizeof(clienteTexto), "%s (%s)",
                             cliente.nombre, cliente.dni);
                } else {
                    snprintf(clienteTexto, sizeof(clienteTexto),
                             "%s (no encontrado)", alquiler.dniCliente);
                }

                if (encontrados == 0) {
                    printf("%-9s %-26s %-40s %-10s %-10s\n", "Patente",
                           "Marca y modelo", "Cliente", "Inicio",
                           "Prevista");
                    printf("-------------------------------------------------"
                           "---------------------------------------------"
                           "---\n");
                }

                printf("%-9s %-26.26s %-40.40s ", alquiler.patente,
                       marcaModelo, clienteTexto);
                imprimirFecha(alquiler.fechaInicio);
                printf(" ");
                imprimirFecha(alquiler.fechaPrevista);
                printf("\n");
                encontrados++;
            }
        }

        cerrarArchivo(archivo);

        if (encontrados == 0) {
            printf("No hay vehiculos alquilados actualmente.\n");
        } else {
            printf("\nVehiculos alquilados: %d\n", encontrados);
        }
    }
}

/* =========================================================
   ALQUILERES
   ========================================================= */

void alquileresPorFecha(void)
{
    Rango rango;

    mostrarTitulo("ALQUILERES POR RANGO DE FECHAS");

    if (pedirRangoFechas(&rango.desde, &rango.hasta)) {
        printf("\n");
        imprimirPeriodo(rango.desde, rango.hasta);
        listarAlquileresFiltrados(filtroPorFecha, &rango,
                                  "No hay alquileres iniciados dentro del "
                                  "periodo indicado.");
    } else {
        mostrarCancelado();
    }
}

void alquileresPorCliente(void)
{
    Cliente cliente;
    CodigoError error = ERR_OK;
    char dni[12];

    mostrarTitulo("ALQUILERES POR CLIENTE");

    if (!pedirDNI(dni)) {
        error = ERR_CANCELADO;
    } else {
        error = obtenerCliente(dni, &cliente);

        if (error == ERR_NO_ENCONTRADO) {
            printf("Error: no existe un cliente con el DNI %s.\n", dni);
        } else if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_CLIENTES);
        }
    }

    if (error == ERR_OK) {
        printf("\nCliente: %s (DNI %s)\n\n", cliente.nombre, cliente.dni);
        listarAlquileresFiltrados(filtroPorCliente, dni,
                                  "El cliente no tiene alquileres "
                                  "registrados.");
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void alquileresPorVehiculo(void)
{
    Vehiculo vehiculo;
    CodigoError error = ERR_OK;
    char patente[12];

    mostrarTitulo("ALQUILERES POR VEHICULO");

    if (!pedirPatente(patente)) {
        error = ERR_CANCELADO;
    } else {
        error = obtenerVehiculo(patente, &vehiculo);

        if (error == ERR_NO_ENCONTRADO) {
            printf("Error: no existe un vehiculo con la patente %s.\n",
                   patente);
        } else if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    if (error == ERR_OK) {
        printf("\nVehiculo: %s %s (%s)\n\n", vehiculo.marca, vehiculo.modelo,
               vehiculo.patente);
        listarAlquileresFiltrados(filtroPorVehiculo, patente,
                                  "El vehiculo no tiene alquileres "
                                  "registrados.");
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void alquileresPorEstado(void)
{
    EstadoAlquiler estado;
    int opcion;

    mostrarTitulo("ALQUILERES POR ESTADO");
    printf("1. Activos\n");
    printf("2. Finalizados\n");
    printf("3. Cancelados\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione el estado: ");

    if (opcion >= 1 && opcion <= 3) {
        estado = (EstadoAlquiler)(opcion - 1);
        printf("\nAlquileres %s:\n\n", textoEstadoAlquiler(estado));
        listarAlquileresFiltrados(filtroPorEstado, &estado,
                                  "No hay alquileres con ese estado.");
    } else if (opcion != 0) {
        printf("Error: opcion invalida.\n");
    }
}

void alquileresPorImporte(void)
{
    FiltroImporte filtro;
    int opcion;

    mostrarTitulo("ALQUILERES POR IMPORTE");

    if (!pedirMonto("Importe de referencia", 0.0f, &filtro.importe)) {
        mostrarCancelado();
    } else {
        printf("\n1. Alquileres con importe SUPERIOR a $%.2f\n",
               filtro.importe);
        printf("2. Alquileres con importe INFERIOR a $%.2f\n", filtro.importe);
        printf("0. Volver\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        if (opcion == 1 || opcion == 2) {
            filtro.mayor = (opcion == 1);
            printf("\n");
            listarAlquileresFiltrados(filtroPorImporte, &filtro,
                                      "No hay alquileres que cumplan esa "
                                      "condicion.");
        } else if (opcion != 0) {
            printf("Error: opcion invalida.\n");
        }
    }
}

/* =========================================================
   PAGOS Y DEUDAS
   ========================================================= */

void pagosPorPeriodo(void)
{
    FILE *archivo;
    Movimiento movimiento;
    Rango rango;
    double total = 0.0;
    int encontrados = 0;

    mostrarTitulo("PAGOS REALIZADOS EN UN PERIODO");

    if (!pedirRangoFechas(&rango.desde, &rango.hasta)) {
        mostrarCancelado();
    } else {
        archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

        if (archivo == NULL) {
            mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_MOVIMIENTOS);
        } else {
            printf("\n");
            imprimirPeriodo(rango.desde, rango.hasta);

            while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
                if (movimiento.tipo == MOV_PAGO &&
                    fechaEnRango(movimiento.fecha, rango.desde,
                                 rango.hasta)) {
                    if (encontrados == 0) {
                        imprimirEncabezadoMovimientos();
                    }

                    imprimirFilaMovimiento(&movimiento);
                    total += movimiento.monto;
                    encontrados++;
                }
            }

            cerrarArchivo(archivo);

            if (encontrados == 0) {
                printf("No hay pagos registrados en el periodo indicado.\n");
            } else {
                printf("\nPagos encontrados: %d - Total cobrado: $%.2f\n",
                       encontrados, total);
            }
        }
    }
}

void clientesConDeuda(void)
{
    FILE *archivo;
    Alquiler alquiler;
    Cliente cliente;
    char nombre[80];
    double totalDeuda = 0.0;
    float saldo;
    int encontrados = 0;

    mostrarTitulo("CLIENTES CON DEUDA");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_ALQUILERES);
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            saldo = saldoAlquiler(&alquiler);

            if (saldo > 0) {
                if (obtenerCliente(alquiler.dniCliente, &cliente) == ERR_OK) {
                    snprintf(nombre, sizeof(nombre), "%s", cliente.nombre);
                } else {
                    snprintf(nombre, sizeof(nombre),
                             "(cliente no encontrado)");
                }

                if (encontrados == 0) {
                    printf("%-9s %-28s %-6s %12s %12s %12s\n", "DNI",
                           "Nombre", "Alq.", "Total", "Pagado", "Saldo");
                    printf("-------------------------------------------------"
                           "-----------------------------\n");
                }

                printf("%-9s %-28.28s %-6d %12.2f %12.2f %12.2f\n",
                       alquiler.dniCliente, nombre, alquiler.numero,
                       alquiler.importeTotal, alquiler.importePagado, saldo);

                totalDeuda += saldo;
                encontrados++;
            }
        }

        cerrarArchivo(archivo);

        if (encontrados == 0) {
            printf("No hay clientes con deuda.\n");
        } else {
            printf("\nAlquileres con saldo pendiente: %d - Deuda total: "
                   "$%.2f\n", encontrados, totalDeuda);
        }
    }
}

/* =========================================================
   ESTADISTICAS
   ========================================================= */

void estadisticasEmpresa(void)
{
    FILE *archivo;
    Alquiler alquiler;
    Vehiculo vehiculo;
    double cobrado = 0.0;
    double pendiente = 0.0;
    int totalVehiculos = 0;
    int totalAlquileres = 0;
    int disponibles = 0;
    int alquilados = 0;
    int mantenimiento = 0;
    int baja = 0;
    int activos = 0;
    int lecturaOk = 1;

    mostrarTitulo("ESTADISTICAS DE LA EMPRESA");

    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_VEHICULOS);
        lecturaOk = 0;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            totalVehiculos++;

            if (vehiculo.estado == VEHICULO_DISPONIBLE) {
                disponibles++;
            } else if (vehiculo.estado == VEHICULO_ALQUILADO) {
                alquilados++;
            } else if (vehiculo.estado == VEHICULO_MANTENIMIENTO) {
                mantenimiento++;
            } else if (vehiculo.estado == VEHICULO_BAJA) {
                baja++;
            }
        }

        cerrarArchivo(archivo);
    }

    if (lecturaOk) {
        archivo = abrirLectura(ARCHIVO_ALQUILERES);

        if (archivo == NULL) {
            mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_ALQUILERES);
            lecturaOk = 0;
        } else {
            while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                totalAlquileres++;
                cobrado += alquiler.importePagado;
                pendiente += saldoAlquiler(&alquiler);

                if (alquiler.estado == ALQUILER_ACTIVO) {
                    activos++;
                }
            }

            cerrarArchivo(archivo);
        }
    }

    if (lecturaOk) {
        printf("Cantidad total de vehiculos:        %d\n", totalVehiculos);
        printf("  Vehiculos disponibles:            %d\n", disponibles);
        printf("  Vehiculos alquilados:             %d\n", alquilados);
        printf("  Vehiculos en mantenimiento:       %d\n", mantenimiento);
        printf("  Vehiculos dados de baja:          %d\n", baja);
        printf("Cantidad total de alquileres:       %d\n", totalAlquileres);
        printf("  Alquileres activos:               %d\n", activos);
        printf("Importe total cobrado:              $%.2f\n", cobrado);
        printf("Importe total pendiente de cobro:   $%.2f\n", pendiente);
    }
}
