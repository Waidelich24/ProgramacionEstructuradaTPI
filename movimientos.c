#include <stdio.h>
#include <string.h>

#include "movimientos.h"
#include "alquileres.h"
#include "clientes.h"
#include "archivos.h"
#include "utiles.h"

void mostrarTablaMovimientos(void)
{
    printf("%-6s %-6s %-11s %12s  %-10s %-9s %-8s\n", "Nro", "Alq.", "Tipo",
           "Monto", "Fecha", "DNI", "Patente");
}

void mostrarFilaMovimiento(const Movimiento *m)
{
    printf("%-6d %-6d %-11s %12.2f  ", m->numero, m->numeroAlquiler,
           textoTipoMovimiento(m->tipo), m->monto);
    imprimirFecha(m->fecha);
    printf(" %-9s %-8s\n", m->dniCliente, m->patente);
}

/* ---------- Archivo de texto del cliente ---------- */

/* Cada movimiento es una linea: nroMov;nroAlquiler;tipo;monto;fecha;patente */
static void escribirLinea(FILE *archivo, const Movimiento *m)
{
    fprintf(archivo, "%d;%d;%s;%.2f;%02d/%02d/%04d;%s\n", m->numero,
            m->numeroAlquiler, textoTipoMovimiento(m->tipo), m->monto,
            m->fecha.dia, m->fecha.mes, m->fecha.anio, m->patente);
}

/* Agrega una linea al final del archivo de texto del cliente */
static int agregarLinea(const Movimiento *m)
{
    char nombre[80];
    FILE *archivo;
    int ok = 0;

    sprintf(nombre, "%s/movimientos_%s.txt", DIRECTORIO_DATOS, m->dniCliente);
    archivo = fopen(nombre, "a");

    if (archivo != NULL) {
        escribirLinea(archivo, m);
        fclose(archivo);
        ok = 1;
    }

    return ok;
}

int generarHistorialCliente(const char dni[])
{
    FILE *origen;
    FILE *destino;
    Movimiento movimiento;
    char nombre[80];
    int ok = 0;

    sprintf(nombre, "%s/movimientos_%s.txt", DIRECTORIO_DATOS, dni);
    origen = abrirLectura(ARCHIVO_MOVIMIENTOS);

    if (origen == NULL) {
        printf("Error: no se pudo abrir el archivo de movimientos.\n");
    } else {
        destino = fopen(nombre, "w");

        if (destino == NULL) {
            printf("Error: no se pudo crear el archivo %s\n", nombre);
        } else {
            while (leerSiguiente(origen, &movimiento, sizeof(Movimiento))) {
                if (strcmp(movimiento.dniCliente, dni) == 0) {
                    escribirLinea(destino, &movimiento);
                }
            }
            fclose(destino);
            ok = 1;
        }
        cerrarArchivo(origen);
    }

    return ok;
}

/* ---------- Registro de movimientos ---------- */

int registrarMovimiento(Movimiento movimiento)
{
    long cantidad = contarRegistros(ARCHIVO_MOVIMIENTOS, sizeof(Movimiento));
    Movimiento ultimo;
    int ok = 1;

    if (movimiento.tipo == MOV_PAGO && movimiento.monto <= 0) {
        printf("Error: el monto de un pago debe ser mayor a cero.\n");
        ok = 0;
    } else if (cantidad < 0) {
        printf("Error: no se pudo abrir el archivo de movimientos.\n");
        ok = 0;
    } else if (cantidad == 0) {
        movimiento.numero = 1;
    } else if (!leerRegistroEn(ARCHIVO_MOVIMIENTOS, &ultimo, sizeof(Movimiento), cantidad - 1)) {
        printf("Error: no se pudo leer el ultimo movimiento del archivo.\n");
        ok = 0;
    } else if (compararFechas(movimiento.fecha, ultimo.fecha) < 0) {
        /* los movimientos tienen que quedar en orden de fecha */
        printf("Error: la fecha de la maquina es anterior al ultimo movimiento registrado (");
        imprimirFecha(ultimo.fecha);
        printf("). Revise la fecha del sistema.\n");
        ok = 0;
    } else {
        /* el numero nuevo es el del ultimo movimiento mas uno */
        movimiento.numero = ultimo.numero + 1;
    }

    if (ok && !agregarRegistro(ARCHIVO_MOVIMIENTOS, &movimiento, sizeof(Movimiento))) {
        printf("Error: no se pudo guardar el movimiento en el archivo.\n");
        ok = 0;
    }

    if (ok && !agregarLinea(&movimiento)) {
        printf("ADVERTENCIA: no se pudo escribir en el archivo de texto del cliente.\n");
    }

    return ok;
}

int mostrarHistorialCliente(const char dni[])
{
    FILE *archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);
    Movimiento movimiento;
    int cantidad = 0;

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de movimientos.\n");
        cantidad = -1;
    } else {
        while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
            if (strcmp(movimiento.dniCliente, dni) == 0) {
                if (cantidad == 0) {
                    mostrarTablaMovimientos();
                }
                mostrarFilaMovimiento(&movimiento);
                cantidad++;
            }
        }
        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("El cliente no tiene operaciones registradas.\n");
        }
    }

    return cantidad;
}

/* ---------- Pagos ---------- */

/* "seguir" vale 1 mientras no haya habido ningun problema */
void registrarPago(void)
{
    FILE *archivo;
    Alquiler alquiler;
    Movimiento movimiento;
    Fecha hoy;
    long pos = -1;
    int seguir = 1;
    int pendientes = 0;
    int numero;
    int listo = 0;
    float saldo = 0;
    float monto = 0;
    float pagadoAntes = 0;

    mostrarTitulo("REGISTRAR PAGO");
    hoy = fechaActual();
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
        seguir = 0;
    } else {
        /* se muestran los alquileres que todavia tienen saldo */
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (saldoAlquiler(&alquiler) > 0) {
                if (pendientes == 0) {
                    printf("Alquileres con saldo pendiente:\n");
                    mostrarTablaAlquileres();
                }
                mostrarFilaAlquiler(&alquiler);
                pendientes++;
            }
        }
        cerrarArchivo(archivo);

        if (pendientes == 0) {
            printf("No hay alquileres con saldo pendiente.\n");
            seguir = 0;
        }
    }

    while (seguir && !listo) {
        if (!pedirEntero("\nNumero de alquiler a pagar", 1, 999999, &numero)) {
            mostrarCancelado();
            seguir = 0;
        } else {
            pos = buscarAlquiler(numero, &alquiler);

            if (pos < -1) {
                printf("Error: no se pudo abrir el archivo de alquileres.\n");
                seguir = 0;
            } else if (pos == -1) {
                printf("Error: no existe el alquiler %d\n", numero);
            } else if (saldoAlquiler(&alquiler) <= 0) {
                printf("Error: el alquiler %d no tiene saldo pendiente (esta %s).\n", numero,
                       textoEstadoAlquiler(alquiler.estado));
            } else {
                listo = 1;
            }
        }
    }

    if (seguir && !fechaMaquinaCorrecta(&alquiler, hoy)) {
        seguir = 0;
    }

    if (seguir) {
        saldo = saldoAlquiler(&alquiler);
        printf("\nImporte total:   $%.2f\n", alquiler.importeTotal);
        printf("Importe pagado:  $%.2f\n", alquiler.importePagado);
        printf("Saldo pendiente: $%.2f\n", saldo);
        printf("Fecha del pago:  ");
        imprimirFecha(hoy);
        printf("\n");

        if (!pedirMonto("Monto a pagar (total o parcial)", saldo, &monto)) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    /* el pago se suma directamente en el alquiler que esta en el archivo */
    if (seguir) {
        pagadoAntes = alquiler.importePagado;
        alquiler.importePagado = alquiler.importePagado + monto;

        if (alquiler.importeTotal - alquiler.importePagado < 0.005) {
            alquiler.importePagado = alquiler.importeTotal;
        }

        if (!escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler, sizeof(Alquiler), pos)) {
            printf("Error: no se pudo guardar el alquiler en el archivo.\n");
            seguir = 0;
        }
    }

    if (seguir) {
        memset(&movimiento, 0, sizeof(Movimiento));
        movimiento.numeroAlquiler = alquiler.numero;
        movimiento.tipo = MOV_PAGO;
        movimiento.monto = monto;
        movimiento.fecha = hoy;
        strcpy(movimiento.dniCliente, alquiler.dniCliente);
        strcpy(movimiento.patente, alquiler.patente);

        if (!registrarMovimiento(movimiento)) {
            /* no se pudo registrar el movimiento: se deja el alquiler como estaba */
            alquiler.importePagado = pagadoAntes;

            if (!escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler, sizeof(Alquiler), pos)) {
                printf("ATENCION: no se pudo deshacer el pago. Revise los datos.\n");
            } else {
                printf("El pago no se registro.\n");
            }
            seguir = 0;
        }
    }

    if (seguir) {
        printf("\nPago de $%.2f registrado. Saldo pendiente: $%.2f\n", monto, saldoAlquiler(&alquiler));
    }
}

/* ---------- Consultas de movimientos ---------- */

void consultarMovimientos(void)
{
    FILE *archivo;
    Movimiento movimiento;
    int cantidad = 0;

    mostrarTitulo("TODOS LOS MOVIMIENTOS");
    archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de movimientos.\n");
    } else {
        mostrarTablaMovimientos();

        while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
            mostrarFilaMovimiento(&movimiento);
            cantidad++;
        }

        cerrarArchivo(archivo);
        printf("\nTotal de movimientos: %d\n", cantidad);
    }
}

static void buscarMovimientos(void)
{
    FILE *archivo;
    Movimiento movimiento;
    char texto[12];
    int seguir = 1;
    int opcion;
    int numero = 0;
    int coincide;
    int encontrados = 0;

    mostrarTitulo("BUSCAR MOVIMIENTOS");
    printf("1. Por numero de alquiler\n2. Por DNI del cliente\n3. Por patente\n0. Volver\n");
    opcion = leerOpcion("Opcion: ");
    texto[0] = '\0';

    if (opcion == 1) {
        if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
            mostrarCancelado();
            seguir = 0;
        }
    } else if (opcion == 2) {
        if (!pedirDNI(texto)) {
            mostrarCancelado();
            seguir = 0;
        }
    } else if (opcion == 3) {
        if (!pedirPatente(texto)) {
            mostrarCancelado();
            seguir = 0;
        }
    } else {
        if (opcion != 0) {
            printf("Error: opcion invalida.\n");
        }
        seguir = 0;
    }

    if (seguir) {
        archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

        if (archivo == NULL) {
            printf("Error: no se pudo abrir el archivo de movimientos.\n");
        } else {
            while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
                coincide = 0;

                if (opcion == 1 && movimiento.numeroAlquiler == numero) {
                    coincide = 1;
                } else if (opcion == 2 && strcmp(movimiento.dniCliente, texto) == 0) {
                    coincide = 1;
                } else if (opcion == 3 && strcmp(movimiento.patente, texto) == 0) {
                    coincide = 1;
                }

                if (coincide) {
                    if (encontrados == 0) {
                        mostrarTablaMovimientos();
                    }
                    mostrarFilaMovimiento(&movimiento);
                    encontrados++;
                }
            }
            cerrarArchivo(archivo);

            if (encontrados == 0) {
                printf("No se encontraron movimientos.\n");
            }
        }
    }
}

static void actualizarArchivoCliente(void)
{
    Cliente cliente;
    char dni[12];
    int resultado;

    mostrarTitulo("ACTUALIZAR ARCHIVO DE UN CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
    } else {
        resultado = obtenerCliente(dni, &cliente);

        if (resultado == 0) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
        } else if (resultado == 1 && generarHistorialCliente(dni)) {
            printf("Se actualizo el archivo %s/movimientos_%s.txt\n", DIRECTORIO_DATOS, dni);
        }
    }
}

void menuMovimientos(void)
{
    int opcion;

    do {
        printf("\n========== PAGOS Y MOVIMIENTOS ==========\n");
        printf("1. Registrar pago\n");
        printf("2. Ver todos los movimientos\n");
        printf("3. Buscar movimientos\n");
        printf("4. Actualizar archivo de movimientos de un cliente\n");
        printf("0. Volver\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 1) {
            registrarPago();
        } else if (opcion == 2) {
            consultarMovimientos();
        } else if (opcion == 3) {
            buscarMovimientos();
        } else if (opcion == 4) {
            actualizarArchivoCliente();
        } else if (opcion != 0) {
            printf("Opcion invalida.\n");
        }
    } while (opcion != 0);
}
