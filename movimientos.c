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

<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        }
        cerrarArchivo(origen);
    }

    return ok;
}

<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

int mostrarHistorialCliente(const char dni[])
{
    FILE *archivo = abrirLectura(ARCHIVO_MOVIMIENTOS);
    Movimiento movimiento;
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
            }
        }
        cerrarArchivo(archivo);

<<<<<<< HEAD
        if (cantidad == 0) {
            printf("El cliente no tiene operaciones registradas.\n");
        }
    }

    return cantidad;
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

/* ---------- Pagos ---------- */

/* "seguir" vale 1 mientras no haya habido ningun problema */
void registrarPago(void)
{
<<<<<<< HEAD
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
=======
    Alquiler alquiler;
    Movimiento movimiento;
    Fecha fecha;
    CodigoError error;
    long posicion = -1;
    float importeAnterior = 0.0f;
    float saldo = 0.0f;
    float monto = 0.0f;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    mostrarTitulo("REGISTRAR PAGO");
    hoy = fechaActual();
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        memset(&movimiento, 0, sizeof(Movimiento));
        movimiento.numeroAlquiler = alquiler.numero;
        movimiento.tipo = MOV_PAGO;
        movimiento.monto = monto;
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
<<<<<<< HEAD
        printf("Error: no se pudo abrir el archivo de movimientos.\n");
    } else {
        mostrarTablaMovimientos();

        while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
            mostrarFilaMovimiento(&movimiento);
=======
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_MOVIMIENTOS);
    } else {
        while (leerSiguiente(archivo, &movimiento, sizeof(Movimiento))) {
            if (cantidad == 0) {
                imprimirEncabezadoMovimientos();
            }

            imprimirFilaMovimiento(&movimiento);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
            cantidad++;
        }

        cerrarArchivo(archivo);
<<<<<<< HEAD
        printf("\nTotal de movimientos: %d\n", cantidad);
=======

        if (cantidad == 0) {
            printf("No hay movimientos registrados.\n");
        } else {
            printf("\nTotal de movimientos: %d\n", cantidad);
        }
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

static void buscarMovimientos(void)
{
    FILE *archivo;
    Movimiento movimiento;
<<<<<<< HEAD
    char texto[12];
    int seguir = 1;
=======
    CodigoError error = ERR_OK;
    char texto[12];
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    int opcion;
    int numero = 0;
    int coincide;
    int encontrados = 0;

    mostrarTitulo("BUSCAR MOVIMIENTOS");
<<<<<<< HEAD
    printf("1. Por numero de alquiler\n2. Por DNI del cliente\n3. Por patente\n0. Volver\n");
    opcion = leerOpcion("Opcion: ");
=======
    printf("1. Por numero de alquiler\n");
    printf("2. Por DNI del cliente\n");
    printf("3. Por patente del vehiculo\n");
    printf("4. Por tipo de movimiento\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione una opcion: ");
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    texto[0] = '\0';

    if (opcion == 0) {
        error = ERR_CANCELADO;
    } else if (opcion < 1 || opcion > 4) {
        error = ERR_DATO_INVALIDO;
        printf("Error: opcion invalida.\n");
    } else if (opcion == 1) {
        if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
                    encontrados++;
                }
            }
            cerrarArchivo(archivo);

<<<<<<< HEAD
            if (encontrados == 0) {
                printf("No se encontraron movimientos.\n");
            }
        }
    }
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

static void actualizarArchivoCliente(void)
{
    Cliente cliente;
    CodigoError error = ERR_OK;
    char dni[12];
    int resultado;

    mostrarTitulo("ACTUALIZAR ARCHIVO DE UN CLIENTE");

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
<<<<<<< HEAD
    } else {
        resultado = obtenerCliente(dni, &cliente);

        if (resultado == 0) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
        } else if (resultado == 1 && generarHistorialCliente(dni)) {
            printf("Se actualizo el archivo %s/movimientos_%s.txt\n", DIRECTORIO_DATOS, dni);
        }
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
