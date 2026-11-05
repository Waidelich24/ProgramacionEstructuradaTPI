#include <stdio.h>
#include <string.h>

#include "consultas.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

/* Pide dos fechas para las consultas por periodo (la final no puede ser anterior a la inicial) */
static int pedirPeriodo(Fecha *desde, Fecha *hasta)
{
    int valido = 0;
    int cancelado = 0;

    if (!pedirFecha("Fecha inicial", desde)) {
        cancelado = 1;
    }

    while (!cancelado && !valido) {
        if (!pedirFecha("Fecha final", hasta)) {
            cancelado = 1;
        } else if (compararFechas(*desde, *hasta) > 0) {
            printf("Error: la fecha final no puede ser anterior a la inicial.\n");
        } else {
            valido = 1;
        }
    }

    return valido;
}

void menuConsultas(void)
{
    int opcion;

    do {
        printf("\n========== CONSULTAS Y REPORTES ==========\n");
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
        printf(" 0. Volver\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 1) {
            vehiculosDisponibles();
        } else if (opcion == 2) {
            vehiculosAlquilados();
        } else if (opcion == 3) {
            alquileresPorFecha();
        } else if (opcion == 4) {
            alquileresPorCliente();
        } else if (opcion == 5) {
            alquileresPorVehiculo();
        } else if (opcion == 6) {
            alquileresPorEstado();
        } else if (opcion == 7) {
            alquileresPorImporte();
        } else if (opcion == 8) {
            pagosPorPeriodo();
        } else if (opcion == 9) {
            clientesConDeuda();
        } else if (opcion == 10) {
            estadisticasEmpresa();
        } else if (opcion != 0) {
            printf("Opcion invalida.\n");
        }
    } while (opcion != 0);
}

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
    char nombre[80];
    int cantidad = 0;

    mostrarTitulo("VEHICULOS ALQUILADOS");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.estado == ALQUILER_ACTIVO) {
                if (obtenerVehiculo(alquiler.patente, &vehiculo) == 1) {
                    sprintf(marcaModelo, "%s %s", vehiculo.marca, vehiculo.modelo);
                } else {
                    strcpy(marcaModelo, "(no encontrado)");
                }

                if (obtenerCliente(alquiler.dniCliente, &cliente) == 1) {
                    strcpy(nombre, cliente.nombre);
                } else {
                    strcpy(nombre, "(no encontrado)");
                }

                if (cantidad == 0) {
                    printf("%-9s %-24s %-9s %-26s %-10s %-10s\n", "Patente",
                           "Marca y modelo", "DNI", "Cliente", "Inicio", "Prevista");
                }

                printf("%-9s %-24.24s %-9s %-26.26s ", alquiler.patente, marcaModelo,
                       alquiler.dniCliente, nombre);
                imprimirFecha(alquiler.fechaInicio);
                printf(" ");
                imprimirFecha(alquiler.fechaPrevista);
                printf("\n");
                cantidad++;
            }
        }
        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay vehiculos alquilados.\n");
        } else {
            printf("\nVehiculos alquilados: %d\n", cantidad);
        }
    }
}

void alquileresPorFecha(void)
{
    FILE *archivo;
    Alquiler alquiler;
    Fecha desde;
    Fecha hasta;
    int cantidad = 0;

    mostrarTitulo("ALQUILERES POR RANGO DE FECHAS");

    if (!pedirPeriodo(&desde, &hasta)) {
        mostrarCancelado();
    } else {
        archivo = abrirLectura(ARCHIVO_ALQUILERES);

        if (archivo == NULL) {
            printf("Error: no se pudo abrir el archivo de alquileres.\n");
        } else {
            printf("\nAlquileres iniciados entre ");
            imprimirFecha(desde);
            printf(" y ");
            imprimirFecha(hasta);
            printf(":\n");

            while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                if (compararFechas(alquiler.fechaInicio, desde) >= 0 &&
                    compararFechas(alquiler.fechaInicio, hasta) <= 0) {
                    if (cantidad == 0) {
                        mostrarTablaAlquileres();
                    }
                    mostrarFilaAlquiler(&alquiler);
                    cantidad++;
                }
            }
            cerrarArchivo(archivo);

            if (cantidad == 0) {
                printf("No hay alquileres en ese periodo.\n");
            } else {
                printf("\nAlquileres encontrados: %d\n", cantidad);
            }
        }
    }
}

void alquileresPorCliente(void)
{
    FILE *archivo;
    Alquiler alquiler;
    Cliente cliente;
    char dni[12];
    int resultado;
    int cantidad = 0;

    mostrarTitulo("ALQUILERES POR CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
    } else {
        resultado = obtenerCliente(dni, &cliente);

        if (resultado == 0) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
        } else if (resultado == 1) {
            archivo = abrirLectura(ARCHIVO_ALQUILERES);

            if (archivo == NULL) {
                printf("Error: no se pudo abrir el archivo de alquileres.\n");
            } else {
                printf("\nCliente: %s (DNI %s)\n", cliente.nombre, cliente.dni);

                while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                    if (strcmp(alquiler.dniCliente, dni) == 0) {
                        if (cantidad == 0) {
                            mostrarTablaAlquileres();
                        }
                        mostrarFilaAlquiler(&alquiler);
                        cantidad++;
                    }
                }
                cerrarArchivo(archivo);

                if (cantidad == 0) {
                    printf("El cliente no tiene alquileres.\n");
                }
            }
        }
    }
}

void alquileresPorVehiculo(void)
{
    FILE *archivo;
    Alquiler alquiler;
    Vehiculo vehiculo;
    char patente[12];
    int resultado;
    int cantidad = 0;

    mostrarTitulo("ALQUILERES POR VEHICULO");

    if (!pedirPatente(patente)) {
        mostrarCancelado();
    } else {
        resultado = obtenerVehiculo(patente, &vehiculo);

        if (resultado == 0) {
            printf("Error: no existe un vehiculo con la patente %s\n", patente);
        } else if (resultado == 1) {
            archivo = abrirLectura(ARCHIVO_ALQUILERES);

            if (archivo == NULL) {
                printf("Error: no se pudo abrir el archivo de alquileres.\n");
            } else {
                printf("\nVehiculo: %s %s (%s)\n", vehiculo.marca, vehiculo.modelo, vehiculo.patente);

                while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                    if (strcmp(alquiler.patente, patente) == 0) {
                        if (cantidad == 0) {
                            mostrarTablaAlquileres();
                        }
                        mostrarFilaAlquiler(&alquiler);
                        cantidad++;
                    }
                }
                cerrarArchivo(archivo);

                if (cantidad == 0) {
                    printf("El vehiculo no tiene alquileres.\n");
                }
            }
        }
    }
}

void alquileresPorEstado(void)
{
    FILE *archivo;
    Alquiler alquiler;
    int opcion;
    int cantidad = 0;

    mostrarTitulo("ALQUILERES POR ESTADO");
    printf("1. Activos\n2. Finalizados\n3. Cancelados\n0. Volver\n");
    opcion = leerOpcion("Opcion: ");

    if (opcion >= 1 && opcion <= 3) {
        archivo = abrirLectura(ARCHIVO_ALQUILERES);

        if (archivo == NULL) {
            printf("Error: no se pudo abrir el archivo de alquileres.\n");
        } else {
            while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                /* opcion 1 = activo (0), 2 = finalizado (1), 3 = cancelado (2) */
                if ((int)alquiler.estado == opcion - 1) {
                    if (cantidad == 0) {
                        mostrarTablaAlquileres();
                    }
                    mostrarFilaAlquiler(&alquiler);
                    cantidad++;
                }
            }
            cerrarArchivo(archivo);

            if (cantidad == 0) {
                printf("No hay alquileres con ese estado.\n");
            } else {
                printf("\nAlquileres encontrados: %d\n", cantidad);
            }
        }
    } else if (opcion != 0) {
        printf("Error: opcion invalida.\n");
    }
}

void alquileresPorImporte(void)
{
    FILE *archivo;
    Alquiler alquiler;
    float importe;
    int opcion;
    int coincide;
    int cantidad = 0;

    mostrarTitulo("ALQUILERES POR IMPORTE");

    if (!pedirMonto("Importe", 0, &importe)) {
        mostrarCancelado();
    } else {
        printf("1. Alquileres con importe mayor a $%.2f\n", importe);
        printf("2. Alquileres con importe menor a $%.2f\n", importe);
        printf("0. Volver\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 1 || opcion == 2) {
            archivo = abrirLectura(ARCHIVO_ALQUILERES);

            if (archivo == NULL) {
                printf("Error: no se pudo abrir el archivo de alquileres.\n");
            } else {
                while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                    coincide = 0;

                    if (opcion == 1 && alquiler.importeTotal > importe) {
                        coincide = 1;
                    } else if (opcion == 2 && alquiler.importeTotal < importe) {
                        coincide = 1;
                    }

                    if (coincide) {
                        if (cantidad == 0) {
                            mostrarTablaAlquileres();
                        }
                        mostrarFilaAlquiler(&alquiler);
                        cantidad++;
                    }
                }
                cerrarArchivo(archivo);

                if (cantidad == 0) {
                    printf("No hay alquileres que cumplan la condicion.\n");
                } else {
                    printf("\nAlquileres encontrados: %d\n", cantidad);
                }
            }
        } else if (opcion != 0) {
            printf("Error: opcion invalida.\n");
        }
    }
}

void pagosPorPeriodo(void)
{
    FILE *archivo;
    Movimiento movimiento;
    Fecha desde;
    Fecha hasta;
    float total = 0;
    int cantidad = 0;

    mostrarTitulo("PAGOS REALIZADOS EN UN PERIODO");

    if (!pedirPeriodo(&desde, &hasta)) {
        mostrarCancelado();
    } else {
        archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

        if (archivo == NULL) {
            printf("Error: no se pudo abrir el archivo de movimientos.\n");
        } else {
            while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
                if (movimiento.tipo == MOV_PAGO &&
                    compararFechas(movimiento.fecha, desde) >= 0 &&
                    compararFechas(movimiento.fecha, hasta) <= 0) {
                    if (cantidad == 0) {
                        printf("\n");
                        mostrarTablaMovimientos();
                    }
                    mostrarFilaMovimiento(&movimiento);
                    total = total + movimiento.monto;
                    cantidad++;
                }
            }
            cerrarArchivo(archivo);

            if (cantidad == 0) {
                printf("No hay pagos en ese periodo.\n");
            } else {
                printf("\nPagos encontrados: %d - Total cobrado: $%.2f\n", cantidad, total);
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
    float saldo;
    float totalDeuda = 0;
    int cantidad = 0;

    mostrarTitulo("CLIENTES CON DEUDA");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            saldo = saldoAlquiler(&alquiler);

            if (saldo > 0) {
                if (obtenerCliente(alquiler.dniCliente, &cliente) == 1) {
                    strcpy(nombre, cliente.nombre);
                } else {
                    strcpy(nombre, "(no encontrado)");
                }

                if (cantidad == 0) {
                    printf("%-9s %-28s %-6s %12s %12s %12s\n", "DNI", "Nombre",
                           "Alq.", "Total", "Pagado", "Saldo");
                }

                printf("%-9s %-28.28s %-6d %12.2f %12.2f %12.2f\n", alquiler.dniCliente,
                       nombre, alquiler.numero, alquiler.importeTotal,
                       alquiler.importePagado, saldo);
                totalDeuda = totalDeuda + saldo;
                cantidad++;
            }
        }
        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay clientes con deuda.\n");
        } else {
            printf("\nAlquileres con deuda: %d - Deuda total: $%.2f\n", cantidad, totalDeuda);
        }
    }
}

void estadisticasEmpresa(void)
{
    FILE *archivo;
    Vehiculo vehiculo;
    Alquiler alquiler;
    float cobrado = 0;
    float pendiente = 0;
    int totalVehiculos = 0;
    int disponibles = 0;
    int alquilados = 0;
    int totalAlquileres = 0;
    int activos = 0;
    int ok = 1;

    mostrarTitulo("ESTADISTICAS DE LA EMPRESA");
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
        ok = 0;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            totalVehiculos++;

            if (vehiculo.estado == VEHICULO_DISPONIBLE) {
                disponibles++;
            } else if (vehiculo.estado == VEHICULO_ALQUILADO) {
                alquilados++;
            }
        }
        cerrarArchivo(archivo);
    }

    if (ok) {
        archivo = abrirLectura(ARCHIVO_ALQUILERES);

        if (archivo == NULL) {
            printf("Error: no se pudo abrir el archivo de alquileres.\n");
            ok = 0;
        } else {
            while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                totalAlquileres++;
                cobrado = cobrado + alquiler.importePagado;
                pendiente = pendiente + saldoAlquiler(&alquiler);

                if (alquiler.estado == ALQUILER_ACTIVO) {
                    activos++;
                }
            }
            cerrarArchivo(archivo);
        }
    }

    if (ok) {
        printf("Cantidad total de vehiculos:      %d\n", totalVehiculos);
        printf("Vehiculos disponibles:            %d\n", disponibles);
        printf("Vehiculos alquilados:             %d\n", alquilados);
        printf("Cantidad total de alquileres:     %d\n", totalAlquileres);
        printf("Alquileres activos:               %d\n", activos);
        printf("Importe total cobrado:            $%.2f\n", cobrado);
        printf("Importe total pendiente de cobro: $%.2f\n", pendiente);
    }
}
