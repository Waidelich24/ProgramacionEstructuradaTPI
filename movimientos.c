#include <stdio.h>
#include <string.h>

#include "movimientos.h"
#include "alquileres.h"
#include "clientes.h"
#include "vehiculos.h"
#include "archivos.h"
#include "utiles.h"

#define TOLERANCIA 0.005f

/* =========================================================
   PRESENTACION
   ========================================================= */

void imprimirEncabezadoMovimientos(void)
{
    printf("%-6s %-6s %-11s %12s  %-10s %-9s %-8s\n",
           "Nro", "Alq.", "Tipo", "Monto", "Fecha", "DNI", "Patente");
    printf("---------------------------------------------------------"
           "-----------------\n");
}

void imprimirFilaMovimiento(const Movimiento *m)
{
    printf("%-6d %-6d %-11s %12.2f  ", m->numero, m->numeroAlquiler,
           textoTipoMovimiento(m->tipo), m->monto);
    imprimirFecha(m->fecha);
    printf(" %-9s %-8s\n", m->dniCliente, m->patente);
}

/* =========================================================
   ARCHIVO DE TEXTO DEL CLIENTE
   ========================================================= */

/* Un movimiento por linea, campos separados por ';':
   nroMov;nroAlquiler;tipo;monto;fecha;patente */
static void escribirLineaHistorial(FILE *archivo, const Movimiento *m)
{
    fprintf(archivo, "%d;%d;%s;%.2f;%02d/%02d/%04d;%s\n",
            m->numero, m->numeroAlquiler, textoTipoMovimiento(m->tipo),
            m->monto, m->fecha.dia, m->fecha.mes, m->fecha.anio, m->patente);
}

static int nombreArchivoHistorial(const char dni[], char nombre[], size_t tam)
{
    return snprintf(nombre, tam, "%s/movimientos_%s.txt",
                    DIRECTORIO_DATOS, dni) < (int)tam;
}

/* Agrega la linea del movimiento al final del archivo del cliente */
static int agregarLineaHistorial(const Movimiento *m)
{
    char nombre[80];
    FILE *archivo;
    int ok = 0;

    if (nombreArchivoHistorial(m->dniCliente, nombre, sizeof(nombre))) {
        archivo = fopen(nombre, "a");

        if (archivo != NULL) {
            escribirLineaHistorial(archivo, m);
            ok = fclose(archivo) == 0;
        }
    }

    return ok;
}

CodigoError generarHistorialCliente(const char dni[])
{
    FILE *origen;
    FILE *destino;
    Movimiento movimiento;
    CodigoError error = ERR_OK;
    char nombre[80];

    if (!nombreArchivoHistorial(dni, nombre, sizeof(nombre))) {
        error = ERR_DATO_INVALIDO;
        mostrarError(error, "DNI demasiado largo para el nombre de archivo");
    } else {
        origen = abrirLectura(ARCHIVO_MOVIMIENTOS);

        if (origen == NULL) {
            error = ERR_ARCHIVO_ABRIR;
            mostrarError(error, ARCHIVO_MOVIMIENTOS);
        } else {
            destino = fopen(nombre, "w");

            if (destino == NULL) {
                error = ERR_ARCHIVO_ESCRIBIR;
                mostrarError(error, nombre);
            } else {
                while (leerSiguiente(origen, &movimiento,
                                     sizeof(Movimiento))) {
                    if (strcmp(movimiento.dniCliente, dni) == 0) {
                        escribirLineaHistorial(destino, &movimiento);
                    }
                }

                if (fclose(destino) != 0) {
                    error = ERR_ARCHIVO_ESCRIBIR;
                    mostrarError(error, nombre);
                }
            }

            cerrarArchivo(origen);
        }
    }

    return error;
}

/* =========================================================
   REGISTRO DE MOVIMIENTOS
   ========================================================= */

/* El numero siguiente es el del ultimo registro mas uno (los movimientos
   solo se agregan al final y nunca se eliminan). */
static CodigoError proximoNumeroMovimiento(int *numero)
{
    long cantidad = contarRegistros(ARCHIVO_MOVIMIENTOS, sizeof(Movimiento));
    Movimiento ultimo;
    CodigoError error = ERR_OK;

    if (cantidad < 0) {
        error = ERR_ARCHIVO_LEER;
    } else if (cantidad == 0) {
        *numero = 1;
    } else {
        error = leerRegistroEn(ARCHIVO_MOVIMIENTOS, &ultimo,
                               sizeof(Movimiento), cantidad - 1);

        if (error == ERR_OK) {
            *numero = ultimo.numero + 1;
        }
    }

    return error;
}

CodigoError registrarMovimiento(Movimiento movimiento)
{
    CodigoError error = ERR_OK;

    if (movimiento.tipo == MOV_PAGO && movimiento.monto <= 0) {
        error = ERR_DATO_INVALIDO;
        printf("Error: el monto de un pago debe ser mayor que cero.\n");
    } else if (!fechaValida(movimiento.fecha)) {
        error = ERR_DATO_INVALIDO;
        printf("Error: la fecha del movimiento no es valida.\n");
    }

    if (error == ERR_OK) {
        error = proximoNumeroMovimiento(&movimiento.numero);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_MOVIMIENTOS);
        }
    }

    if (error == ERR_OK) {
        error = agregarRegistro(ARCHIVO_MOVIMIENTOS, &movimiento,
                                sizeof(Movimiento));

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_MOVIMIENTOS);
        }
    }

    if (error == ERR_OK && !agregarLineaHistorial(&movimiento)) {
        printf("ADVERTENCIA: el movimiento se guardo pero no se pudo "
               "actualizar el archivo de texto del cliente.\n");
    }

    return error;
}

int mostrarHistorialCliente(const char dni[])
{
    FILE *archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);
    Movimiento movimiento;
    int encontrados = 0;

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_MOVIMIENTOS);
        encontrados = -1;
    } else {
        while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
            if (strcmp(movimiento.dniCliente, dni) == 0) {
                if (encontrados == 0) {
                    imprimirEncabezadoMovimientos();
                }

                imprimirFilaMovimiento(&movimiento);
                encontrados++;
            }
        }

        cerrarArchivo(archivo);

        if (encontrados == 0) {
            printf("El cliente no tiene operaciones registradas.\n");
        } else {
            printf("\nTotal de operaciones: %d\n", encontrados);
        }
    }

    return encontrados;
}

/* =========================================================
   REGISTRAR PAGO
   ========================================================= */

/* Un alquiler admite pagos si no esta cancelado y tiene saldo pendiente */
static int admitePago(const Alquiler *alquiler)
{
    return saldoAlquiler(alquiler) > 0;
}

static int coincideBusqueda(const Alquiler *alquiler, int porDNI,
                            const char *clave)
{
    return admitePago(alquiler) &&
           strcmp(porDNI ? alquiler->dniCliente : alquiler->patente,
                  clave) == 0;
}

/* Pide el alquiler a pagar (por numero, DNI o patente).
   Devuelve ERR_OK con la posicion y el registro, o el motivo por el que
   no se eligio ninguno. */
static CodigoError elegirAlquilerConSaldo(long *posicion,
                                          Alquiler *seleccionado)
{
    Cliente cliente;
    Vehiculo vehiculo;
    FILE *archivo;
    Alquiler actual;
    CodigoError error = ERR_OK;
    CodigoError resultado = ERR_OK;
    char clave[12];
    long encontrada;
    int metodo;
    int numero;
    int porDNI;
    int coincidentes = 0;
    int listo = 0;

    printf("Buscar el alquiler por:\n");
    printf("1. Numero de alquiler\n");
    printf("2. DNI del cliente\n");
    printf("3. Patente del vehiculo\n");
    printf("0. Cancelar\n");

    metodo = leerOpcion("Opcion: ");

    if (metodo == 0) {
        error = ERR_CANCELADO;
    } else if (metodo < 1 || metodo > 3) {
        error = ERR_DATO_INVALIDO;
        printf("Error: opcion invalida.\n");
    }

    if (error == ERR_OK && metodo == 1) {
        while (error == ERR_OK && !listo) {
            if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
                error = ERR_CANCELADO;
            } else {
                encontrada = buscarAlquiler(numero, seleccionado);

                if (encontrada < -1) {
                    error = ERR_ARCHIVO_LEER;
                    mostrarError(error, ARCHIVO_ALQUILERES);
                } else if (encontrada == -1) {
                    printf("Error: no existe un alquiler con el numero %d.\n",
                           numero);
                } else if (seleccionado->estado == ALQUILER_CANCELADO) {
                    printf("Error: el alquiler %d esta cancelado.\n", numero);
                } else if (!admitePago(seleccionado)) {
                    printf("El alquiler %d ya esta totalmente pagado.\n",
                           numero);
                } else {
                    *posicion = encontrada;
                    listo = 1;
                }
            }
        }
    } else if (error == ERR_OK) {
        porDNI = (metodo == 2);

        if (porDNI) {
            if (!pedirDNI(clave)) {
                error = ERR_CANCELADO;
            } else {
                resultado = obtenerCliente(clave, &cliente);
            }
        } else if (!pedirPatente(clave)) {
            error = ERR_CANCELADO;
        } else {
            resultado = obtenerVehiculo(clave, &vehiculo);
        }

        if (error == ERR_OK && resultado == ERR_NO_ENCONTRADO) {
            error = resultado;
            printf(porDNI ? "Error: no existe un cliente con ese DNI.\n"
                          : "Error: no existe un vehiculo con esa patente.\n");
        } else if (error == ERR_OK && resultado != ERR_OK) {
            error = resultado;
            mostrarError(error, porDNI ? ARCHIVO_CLIENTES : ARCHIVO_VEHICULOS);
        }

        if (error == ERR_OK) {
            archivo = abrirLectura(ARCHIVO_ALQUILERES);

            if (archivo == NULL) {
                error = ERR_ARCHIVO_ABRIR;
                mostrarError(error, ARCHIVO_ALQUILERES);
            } else {
                while (leerSiguiente(archivo, &actual, sizeof(Alquiler))) {
                    if (coincideBusqueda(&actual, porDNI, clave)) {
                        if (coincidentes == 0) {
                            printf("\nAlquileres con saldo pendiente:\n");
                            imprimirEncabezadoAlquileres();
                        }

                        imprimirFilaAlquiler(&actual);
                        coincidentes++;
                    }
                }

                cerrarArchivo(archivo);

                if (coincidentes == 0) {
                    error = ERR_SIN_DATOS;
                    printf("No hay alquileres con saldo pendiente para ese "
                           "dato.\n");
                }
            }
        }

        if (error == ERR_OK) {
            printf("\n");
        }

        while (error == ERR_OK && !listo) {
            if (!pedirEntero("Numero del alquiler a pagar", 1, 999999,
                             &numero)) {
                error = ERR_CANCELADO;
            } else {
                encontrada = buscarAlquiler(numero, seleccionado);

                if (encontrada < -1) {
                    error = ERR_ARCHIVO_LEER;
                    mostrarError(error, ARCHIVO_ALQUILERES);
                } else if (encontrada >= 0 &&
                           coincideBusqueda(seleccionado, porDNI, clave)) {
                    *posicion = encontrada;
                    listo = 1;
                } else {
                    printf("Error: el numero %d no figura en la lista "
                           "mostrada.\n", numero);
                }
            }
        }
    }

    return error;
}

void registrarPago(void)
{
    Alquiler alquiler;
    Movimiento movimiento;
    Fecha fecha;
    CodigoError error;
    long posicion = -1;
    float importeAnterior = 0.0f;
    float saldo = 0.0f;
    float monto = 0.0f;

    mostrarTitulo("REGISTRAR PAGO");

    error = elegirAlquilerConSaldo(&posicion, &alquiler);

    if (error == ERR_OK) {
        saldo = saldoAlquiler(&alquiler);

        printf("\n--- ALQUILER %d ---\n", alquiler.numero);
        printf("Cliente (DNI):   %s\n", alquiler.dniCliente);
        printf("Vehiculo:        %s\n", alquiler.patente);
        printf("Importe total:   $%.2f\n", alquiler.importeTotal);
        printf("Importe pagado:  $%.2f\n", alquiler.importePagado);
        printf("Saldo pendiente: $%.2f\n\n", saldo);

        if (!pedirMonto("Monto a pagar (total o parcial)", saldo, &monto) ||
            !pedirFechaOperacion("Fecha del pago", &fecha)) {
            error = ERR_CANCELADO;
        }
    }

    /* El pago se aplica directamente sobre el registro del alquiler */
    if (error == ERR_OK) {
        importeAnterior = alquiler.importePagado;
        alquiler.importePagado += monto;

        /* Evita residuos de redondeo cuando se cancela el saldo completo */
        if (alquiler.importeTotal - alquiler.importePagado < TOLERANCIA) {
            alquiler.importePagado = alquiler.importeTotal;
        }

        error = escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler,
                                   sizeof(Alquiler), posicion);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_ALQUILERES);
            printf("El pago no fue registrado.\n");
        }
    }

    if (error == ERR_OK) {
        memset(&movimiento, 0, sizeof(Movimiento));
        movimiento.numeroAlquiler = alquiler.numero;
        movimiento.tipo = MOV_PAGO;
        movimiento.monto = monto;
        movimiento.fecha = fecha;
        strcpy(movimiento.dniCliente, alquiler.dniCliente);
        strcpy(movimiento.patente, alquiler.patente);

        error = registrarMovimiento(movimiento);

        if (error != ERR_OK) {
            alquiler.importePagado = importeAnterior;

            if (escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler,
                                   sizeof(Alquiler), posicion) != ERR_OK) {
                printf("ATENCION: no se pudo restaurar el alquiler. "
                       "Revise los datos.\n");
            } else {
                printf("El pago fallo y se revirtio la actualizacion.\n");
            }
        }
    }

    if (error == ERR_OK) {
        printf("\nPago de $%.2f registrado correctamente.\n", monto);
        printf("Nuevo saldo pendiente: $%.2f\n", saldoAlquiler(&alquiler));
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

/* =========================================================
   CONSULTAS DE MOVIMIENTOS
   ========================================================= */

void consultarMovimientos(void)
{
    FILE *archivo;
    Movimiento movimiento;
    int cantidad = 0;

    mostrarTitulo("TODOS LOS MOVIMIENTOS");
    archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_MOVIMIENTOS);
    } else {
        while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
            if (cantidad == 0) {
                imprimirEncabezadoMovimientos();
            }

            imprimirFilaMovimiento(&movimiento);
            cantidad++;
        }

        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay movimientos registrados.\n");
        } else {
            printf("\nTotal de movimientos: %d\n", cantidad);
        }
    }
}

static void buscarMovimientos(void)
{
    FILE *archivo;
    Movimiento movimiento;
    CodigoError error = ERR_OK;
    char texto[12];
    int opcion;
    int numero = 0;
    int coincide;
    int encontrados = 0;

    mostrarTitulo("BUSCAR MOVIMIENTOS");
    printf("1. Por numero de alquiler\n");
    printf("2. Por DNI del cliente\n");
    printf("3. Por patente del vehiculo\n");
    printf("4. Por tipo de movimiento\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione una opcion: ");
    texto[0] = '\0';

    if (opcion == 0) {
        error = ERR_CANCELADO;
    } else if (opcion < 1 || opcion > 4) {
        error = ERR_DATO_INVALIDO;
        printf("Error: opcion invalida.\n");
    } else if (opcion == 1) {
        if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
            error = ERR_CANCELADO;
        }
    } else if (opcion == 2) {
        if (!pedirDNI(texto)) {
            error = ERR_CANCELADO;
        }
    } else if (opcion == 3) {
        if (!pedirPatente(texto)) {
            error = ERR_CANCELADO;
        }
    } else {
        printf("\n1. Alquiler\n2. Pago\n3. Devolucion\n4. Ajuste\n");

        if (!pedirEntero("Tipo de movimiento", 1, 4, &numero)) {
            error = ERR_CANCELADO;
        } else {
            numero--;
        }
    }

    if (error == ERR_OK) {
        archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);

        if (archivo == NULL) {
            error = ERR_ARCHIVO_ABRIR;
            mostrarError(error, ARCHIVO_MOVIMIENTOS);
        } else {
            while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
                if (opcion == 1) {
                    coincide = movimiento.numeroAlquiler == numero;
                } else if (opcion == 2) {
                    coincide = strcmp(movimiento.dniCliente, texto) == 0;
                } else if (opcion == 3) {
                    coincide = strcmp(movimiento.patente, texto) == 0;
                } else {
                    coincide = (int)movimiento.tipo == numero;
                }

                if (coincide) {
                    if (encontrados == 0) {
                        imprimirEncabezadoMovimientos();
                    }

                    imprimirFilaMovimiento(&movimiento);
                    encontrados++;
                }
            }

            cerrarArchivo(archivo);

            if (encontrados == 0) {
                printf("No se encontraron movimientos con ese criterio.\n");
            } else {
                printf("\nMovimientos encontrados: %d\n", encontrados);
            }
        }
    }

    if (error == ERR_CANCELADO && opcion != 0) {
        mostrarCancelado();
    }
}

/* Saldo de un alquiler puntual o de todos los alquileres de un cliente */
static void consultarSaldo(void)
{
    FILE *archivo;
    Alquiler alquiler;
    CodigoError error = ERR_OK;
    char dni[12];
    int opcion;
    int numero = 0;
    int encontrados = 0;

    mostrarTitulo("CONSULTA DE SALDOS");
    printf("1. Por numero de alquiler\n");
    printf("2. Por DNI del cliente\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione una opcion: ");
    dni[0] = '\0';

    if (opcion == 0) {
        error = ERR_CANCELADO;
    } else if (opcion != 1 && opcion != 2) {
        error = ERR_DATO_INVALIDO;
        printf("Error: opcion invalida.\n");
    } else if (opcion == 1) {
        if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
            error = ERR_CANCELADO;
        }
    } else if (!pedirDNI(dni)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        archivo = abrirLectura(ARCHIVO_ALQUILERES);

        if (archivo == NULL) {
            error = ERR_ARCHIVO_ABRIR;
            mostrarError(error, ARCHIVO_ALQUILERES);
        } else {
            while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
                if ((opcion == 1 && alquiler.numero == numero) ||
                    (opcion == 2 && strcmp(alquiler.dniCliente, dni) == 0)) {
                    if (encontrados == 0) {
                        imprimirEncabezadoAlquileres();
                    }

                    imprimirFilaAlquiler(&alquiler);
                    encontrados++;
                }
            }

            cerrarArchivo(archivo);

            if (encontrados == 0) {
                printf("No se encontraron alquileres con ese dato.\n");
            }
        }
    }

    if (error == ERR_CANCELADO && opcion != 0) {
        mostrarCancelado();
    }
}

static void actualizarArchivoCliente(void)
{
    Cliente cliente;
    CodigoError error = ERR_OK;
    char dni[12];

    mostrarTitulo("ACTUALIZAR ARCHIVO DE MOVIMIENTOS DE UN CLIENTE");

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
        error = generarHistorialCliente(dni);

        if (error == ERR_OK) {
            printf("Archivo %s/movimientos_%s.txt actualizado "
                   "correctamente.\n", DIRECTORIO_DATOS, dni);
        }
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void menuMovimientos(void)
{
    int opcion;

    do {
        printf("\n========================================\n");
        printf("        PAGOS Y MOVIMIENTOS\n");
        printf("========================================\n");
        printf("1. Registrar pago\n");
        printf("2. Consultar todos los movimientos\n");
        printf("3. Buscar movimientos\n");
        printf("4. Consultar saldos\n");
        printf("5. Actualizar archivo de movimientos de un cliente\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1: registrarPago();            break;
            case 2: consultarMovimientos();     break;
            case 3: buscarMovimientos();        break;
            case 4: consultarSaldo();           break;
            case 5: actualizarArchivoCliente(); break;
            case 0:                             break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
