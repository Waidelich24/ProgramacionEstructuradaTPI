#include <stdio.h>
#include <string.h>

#include "consultas.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    return valido;
}

<<<<<<< HEAD
=======
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

>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
<<<<<<< HEAD
    char nombre[80];
    int cantidad = 0;
=======
    char clienteTexto[100];
    int encontrados = 0;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("VEHICULOS ALQUILADOS");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
                imprimirFecha(alquiler.fechaInicio);
                printf(" ");
                imprimirFecha(alquiler.fechaPrevista);
                printf("\n");
<<<<<<< HEAD
                cantidad++;
            }
        }
        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay vehiculos alquilados.\n");
        } else {
            printf("\nVehiculos alquilados: %d\n", cantidad);
=======
                encontrados++;
            }
        }

        cerrarArchivo(archivo);

        if (encontrados == 0) {
            printf("No hay vehiculos alquilados actualmente.\n");
        } else {
            printf("\nVehiculos alquilados: %d\n", encontrados);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        }
    }
}

void alquileresPorFecha(void)
{
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void alquileresPorCliente(void)
{
<<<<<<< HEAD
    FILE *archivo;
    Alquiler alquiler;
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    Cliente cliente;
    CodigoError error = ERR_OK;
    char dni[12];
<<<<<<< HEAD
    int resultado;
    int cantidad = 0;
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("ALQUILERES POR CLIENTE");

    if (!pedirDNI(dni)) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void alquileresPorVehiculo(void)
{
<<<<<<< HEAD
    FILE *archivo;
    Alquiler alquiler;
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    Vehiculo vehiculo;
    CodigoError error = ERR_OK;
    char patente[12];
<<<<<<< HEAD
    int resultado;
    int cantidad = 0;
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("ALQUILERES POR VEHICULO");

    if (!pedirPatente(patente)) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void alquileresPorEstado(void)
{
<<<<<<< HEAD
    FILE *archivo;
    Alquiler alquiler;
    int opcion;
    int cantidad = 0;
=======
    EstadoAlquiler estado;
    int opcion;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("ALQUILERES POR ESTADO");
    printf("1. Activos\n2. Finalizados\n3. Cancelados\n0. Volver\n");
    opcion = leerOpcion("Opcion: ");

    if (opcion >= 1 && opcion <= 3) {
        archivo = abrirLectura(ARCHIVO_ALQUILERES);

<<<<<<< HEAD
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
=======
    if (opcion >= 1 && opcion <= 3) {
        estado = (EstadoAlquiler)(opcion - 1);
        printf("\nAlquileres %s:\n\n", textoEstadoAlquiler(estado));
        listarAlquileresFiltrados(filtroPorEstado, &estado,
                                  "No hay alquileres con ese estado.");
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    } else if (opcion != 0) {
        printf("Error: opcion invalida.\n");
    }
}

void alquileresPorImporte(void)
{
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        } else if (opcion != 0) {
            printf("Error: opcion invalida.\n");
        }
    }
}

void pagosPorPeriodo(void)
{
    FILE *archivo;
    Movimiento movimiento;
<<<<<<< HEAD
    Fecha desde;
    Fecha hasta;
    float total = 0;
    int cantidad = 0;

    mostrarTitulo("PAGOS REALIZADOS EN UN PERIODO");

    if (!pedirPeriodo(&desde, &hasta)) {
=======
    Rango rango;
    double total = 0.0;
    int encontrados = 0;

    mostrarTitulo("PAGOS REALIZADOS EN UN PERIODO");

    if (!pedirRangoFechas(&rango.desde, &rango.hasta)) {
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        mostrarCancelado();
    } else {
        archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

        if (archivo == NULL) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
<<<<<<< HEAD
    float totalDeuda = 0;
    int cantidad = 0;
=======
    int encontrados = 0;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("CLIENTES CON DEUDA");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
<<<<<<< HEAD
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
=======
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_ALQUILERES);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            saldo = saldoAlquiler(&alquiler);

            if (saldo > 0) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        }
    }
}

void estadisticasEmpresa(void)
{
    FILE *archivo;
<<<<<<< HEAD
    Vehiculo vehiculo;
    Alquiler alquiler;
    float cobrado = 0;
    float pendiente = 0;
    int totalVehiculos = 0;
=======
    Alquiler alquiler;
    Vehiculo vehiculo;
    double cobrado = 0.0;
    double pendiente = 0.0;
    int totalVehiculos = 0;
    int totalAlquileres = 0;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    int disponibles = 0;
    int alquilados = 0;
    int totalAlquileres = 0;
    int activos = 0;
<<<<<<< HEAD
    int ok = 1;
=======
    int lecturaOk = 1;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("ESTADISTICAS DE LA EMPRESA");
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

<<<<<<< HEAD
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
        ok = 0;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            totalVehiculos++;

=======
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_VEHICULOS);
        lecturaOk = 0;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            totalVehiculos++;

>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
            if (vehiculo.estado == VEHICULO_DISPONIBLE) {
                disponibles++;
            } else if (vehiculo.estado == VEHICULO_ALQUILADO) {
                alquilados++;
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

                if (alquiler.estado == ALQUILER_ACTIVO) {
                    activos++;
                }
            }
<<<<<<< HEAD
=======

>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
            cerrarArchivo(archivo);
        }
    }

<<<<<<< HEAD
    if (ok) {
        printf("Cantidad total de vehiculos:      %d\n", totalVehiculos);
        printf("Vehiculos disponibles:            %d\n", disponibles);
        printf("Vehiculos alquilados:             %d\n", alquilados);
        printf("Cantidad total de alquileres:     %d\n", totalAlquileres);
        printf("Alquileres activos:               %d\n", activos);
        printf("Importe total cobrado:            $%.2f\n", cobrado);
        printf("Importe total pendiente de cobro: $%.2f\n", pendiente);
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}
