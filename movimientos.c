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
   ARCHIVOS DE MOVIMIENTOS
   ========================================================= */

static int leerMovimientos(Movimiento movimientos[], int *cantidad)
{
    int resultado = leerRegistros(ARCHIVO_MOVIMIENTOS, movimientos,
                                  sizeof(Movimiento), MAX_MOVIMIENTOS);

    if (resultado < 0) {
        printf("Error: no se pudo leer el archivo de movimientos (%s).\n",
               ARCHIVO_MOVIMIENTOS);
        return 0;
    }

    *cantidad = resultado;
    return 1;
}

static int nuevoNumeroMovimiento(const Movimiento movimientos[], int cantidad)
{
    int i;
    int mayor = 0;

    for (i = 0; i < cantidad; i++) {
        if (movimientos[i].numero > mayor) {
            mayor = movimientos[i].numero;
        }
    }

    return mayor + 1;
}

/*
 * Archivo de texto del cliente: un movimiento por linea, con los campos
 * separados por ';':  nroMov;nroAlquiler;tipo;monto;fecha;patente
 */
int generarHistorialCliente(const char dni[])
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    char nombreArchivo[80];
    FILE *archivo;
    int cantidad;
    int i;

    if (!leerMovimientos(movimientos, &cantidad)) {
        return 0;
    }

    if (snprintf(nombreArchivo, sizeof(nombreArchivo),
                 "%s/movimientos_%s.txt", DIRECTORIO_DATOS, dni)
        >= (int)sizeof(nombreArchivo)) {
        printf("Error: el DNI no permite generar el nombre del archivo.\n");
        return 0;
    }

    archivo = fopen(nombreArchivo, "w");

    if (archivo == NULL) {
        printf("Error: no se pudo crear el archivo '%s'.\n", nombreArchivo);
        return 0;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(movimientos[i].dniCliente, dni) == 0) {
            fprintf(archivo, "%d;%d;%s;%.2f;%02d/%02d/%04d;%s\n",
                    movimientos[i].numero,
                    movimientos[i].numeroAlquiler,
                    textoTipoMovimiento(movimientos[i].tipo),
                    movimientos[i].monto,
                    movimientos[i].fecha.dia,
                    movimientos[i].fecha.mes,
                    movimientos[i].fecha.anio,
                    movimientos[i].patente);
        }
    }

    if (fclose(archivo) != 0) {
        printf("Error: no se pudo cerrar '%s'.\n", nombreArchivo);
        return 0;
    }

    return 1;
}

int registrarMovimiento(Movimiento movimiento)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;

    if (movimiento.tipo == MOV_PAGO && movimiento.monto <= 0) {
        printf("Error: el monto de un pago debe ser mayor que cero.\n");
        return 0;
    }

    if (!fechaValida(movimiento.fecha)) {
        printf("Error: la fecha del movimiento no es valida.\n");
        return 0;
    }

    if (!leerMovimientos(movimientos, &cantidad)) {
        return 0;
    }

    if (cantidad >= MAX_MOVIMIENTOS) {
        printf("Error: se alcanzo el limite de %d movimientos.\n",
               MAX_MOVIMIENTOS);
        return 0;
    }

    movimiento.numero = nuevoNumeroMovimiento(movimientos, cantidad);

    if (!agregarRegistro(ARCHIVO_MOVIMIENTOS, &movimiento,
                         sizeof(Movimiento))) {
        printf("Error: no se pudo guardar el movimiento.\n");
        return 0;
    }

    if (!generarHistorialCliente(movimiento.dniCliente)) {
        printf("ADVERTENCIA: el movimiento se guardo pero no se pudo "
               "actualizar el archivo de texto del cliente.\n");
    }

    return 1;
}

int mostrarHistorialCliente(const char dni[])
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;
    int i;
    int encontrados = 0;

    if (!leerMovimientos(movimientos, &cantidad)) {
        return -1;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(movimientos[i].dniCliente, dni) == 0) {
            if (encontrados == 0) {
                imprimirEncabezadoMovimientos();
            }

            imprimirFilaMovimiento(&movimientos[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("El cliente no tiene operaciones registradas.\n");
    } else {
        printf("\nTotal de operaciones: %d\n", encontrados);
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

static int indicePorNumero(const Alquiler alquileres[], int cantidad,
                           int numero)
{
    int i;

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].numero == numero) {
            return i;
        }
    }

    return -1;
}

/* Pide el alquiler a pagar. Devuelve su posicion o -1 si cancela / no hay */
static int elegirAlquilerConSaldo(const Alquiler alquileres[], int cantidad)
{
    Cliente cliente;
    Vehiculo vehiculo;
    char dni[12];
    char patente[12];
    int metodo;
    int numero;
    int indice;
    int i;
    int resultado;
    int coincidentes;
    int porDNI;

    printf("Buscar el alquiler por:\n");
    printf("1. Numero de alquiler\n");
    printf("2. DNI del cliente\n");
    printf("3. Patente del vehiculo\n");
    printf("0. Cancelar\n");

    metodo = leerOpcion("Opcion: ");

    if (metodo < 1 || metodo > 3) {
        if (metodo != 0) {
            printf("Error: opcion invalida.\n");
        }

        return -1;
    }

    if (metodo == 1) {
        while (pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
            indice = indicePorNumero(alquileres, cantidad, numero);

            if (indice < 0) {
                printf("Error: no existe un alquiler con el numero %d.\n",
                       numero);
            } else if (alquileres[indice].estado == ALQUILER_CANCELADO) {
                printf("Error: el alquiler %d esta cancelado.\n", numero);
            } else if (!admitePago(&alquileres[indice])) {
                printf("El alquiler %d ya esta totalmente pagado.\n", numero);
            } else {
                return indice;
            }
        }

        return -1;
    }

    porDNI = (metodo == 2);
    dni[0] = '\0';
    patente[0] = '\0';

    if (porDNI) {
        if (!pedirDNI(dni)) {
            return -1;
        }

        resultado = obtenerCliente(dni, &cliente);
    } else {
        if (!pedirPatente(patente)) {
            return -1;
        }

        resultado = obtenerVehiculo(patente, &vehiculo);
    }

    if (resultado < 0) {
        return -1;
    }

    if (resultado == 0) {
        printf(porDNI ? "Error: no existe un cliente con ese DNI.\n"
                      : "Error: no existe un vehiculo con esa patente.\n");
        return -1;
    }

    coincidentes = 0;

    for (i = 0; i < cantidad; i++) {
        if (admitePago(&alquileres[i]) &&
            strcmp(porDNI ? alquileres[i].dniCliente : alquileres[i].patente,
                   porDNI ? dni : patente) == 0) {
            if (coincidentes == 0) {
                printf("\nAlquileres con saldo pendiente:\n");
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            coincidentes++;
        }
    }

    if (coincidentes == 0) {
        printf("No hay alquileres con saldo pendiente para ese dato.\n");
        return -1;
    }

    printf("\n");

    while (pedirEntero("Numero del alquiler a pagar", 1, 999999, &numero)) {
        indice = indicePorNumero(alquileres, cantidad, numero);

        if (indice >= 0 && admitePago(&alquileres[indice]) &&
            strcmp(porDNI ? alquileres[indice].dniCliente
                          : alquileres[indice].patente,
                   porDNI ? dni : patente) == 0) {
            return indice;
        }

        printf("Error: el numero %d no figura en la lista mostrada.\n",
               numero);
    }

    return -1;
}

void registrarPago(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler *alquiler;
    Movimiento movimiento;
    Fecha fecha;
    float importeAnterior;
    float saldo;
    float monto;
    int cantidad;
    int indice;

    mostrarTitulo("REGISTRAR PAGO");

    cantidad = leerRegistros(ARCHIVO_ALQUILERES, alquileres,
                             sizeof(Alquiler), MAX_ALQUILERES);

    if (cantidad < 0) {
        printf("Error: no se pudo leer el archivo de alquileres.\n");
        return;
    }

    if (cantidad == 0) {
        printf("No hay alquileres registrados.\n");
        return;
    }

    indice = elegirAlquilerConSaldo(alquileres, cantidad);

    if (indice < 0) {
        return;
    }

    alquiler = &alquileres[indice];
    saldo = saldoAlquiler(alquiler);

    printf("\n--- ALQUILER %d ---\n", alquiler->numero);
    printf("Cliente (DNI):   %s\n", alquiler->dniCliente);
    printf("Vehiculo:        %s\n", alquiler->patente);
    printf("Importe total:   $%.2f\n", alquiler->importeTotal);
    printf("Importe pagado:  $%.2f\n", alquiler->importePagado);
    printf("Saldo pendiente: $%.2f\n\n", saldo);

    if (!pedirMonto("Monto a pagar (total o parcial)", saldo, &monto)) {
        mostrarCancelado();
        return;
    }

    if (!pedirFechaOperacion("Fecha del pago", &fecha)) {
        mostrarCancelado();
        return;
    }

    importeAnterior = alquiler->importePagado;
    alquiler->importePagado += monto;

    /* Evita residuos de redondeo cuando se cancela el saldo completo */
    if (alquiler->importeTotal - alquiler->importePagado < TOLERANCIA) {
        alquiler->importePagado = alquiler->importeTotal;
    }

    if (!guardarRegistros(ARCHIVO_ALQUILERES, alquileres,
                          sizeof(Alquiler), cantidad)) {
        printf("Error: no se pudo actualizar el alquiler. "
               "El pago no fue registrado.\n");
        return;
    }

    memset(&movimiento, 0, sizeof(Movimiento));
    movimiento.numeroAlquiler = alquiler->numero;
    movimiento.tipo = MOV_PAGO;
    movimiento.monto = monto;
    movimiento.fecha = fecha;
    strcpy(movimiento.dniCliente, alquiler->dniCliente);
    strcpy(movimiento.patente, alquiler->patente);

    if (!registrarMovimiento(movimiento)) {
        alquiler->importePagado = importeAnterior;

        if (!guardarRegistros(ARCHIVO_ALQUILERES, alquileres,
                              sizeof(Alquiler), cantidad)) {
            printf("ATENCION: no se pudo restaurar el alquiler. "
                   "Revise los datos.\n");
        } else {
            printf("El pago fallo y se revirtio la actualizacion.\n");
        }

        return;
    }

    printf("\nPago de $%.2f registrado correctamente.\n", monto);
    printf("Nuevo saldo pendiente: $%.2f\n", saldoAlquiler(alquiler));
}

/* =========================================================
   CONSULTAS DE MOVIMIENTOS
   ========================================================= */

void consultarMovimientos(void)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;
    int i;

    mostrarTitulo("TODOS LOS MOVIMIENTOS");

    if (!leerMovimientos(movimientos, &cantidad)) {
        return;
    }

    if (cantidad == 0) {
        printf("No hay movimientos registrados.\n");
        return;
    }

    imprimirEncabezadoMovimientos();

    for (i = 0; i < cantidad; i++) {
        imprimirFilaMovimiento(&movimientos[i]);
    }

    printf("\nTotal de movimientos: %d\n", cantidad);
}

static void buscarMovimientos(void)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    char texto[12];
    int cantidad;
    int opcion;
    int numero = 0;
    int i;
    int coincide;
    int encontrados = 0;

    mostrarTitulo("BUSCAR MOVIMIENTOS");
    printf("1. Por numero de alquiler\n");
    printf("2. Por DNI del cliente\n");
    printf("3. Por patente del vehiculo\n");
    printf("4. Por tipo de movimiento\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione una opcion: ");

    if (opcion == 0) {
        return;
    }

    if (opcion < 1 || opcion > 4) {
        printf("Error: opcion invalida.\n");
        return;
    }

    texto[0] = '\0';

    if (opcion == 1) {
        if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
            return;
        }
    } else if (opcion == 2) {
        if (!pedirDNI(texto)) {
            return;
        }
    } else if (opcion == 3) {
        if (!pedirPatente(texto)) {
            return;
        }
    } else {
        printf("\n1. Alquiler\n2. Pago\n3. Devolucion\n4. Ajuste\n");

        if (!pedirEntero("Tipo de movimiento", 1, 4, &numero)) {
            return;
        }

        numero--;
    }

    if (!leerMovimientos(movimientos, &cantidad)) {
        return;
    }

    for (i = 0; i < cantidad; i++) {
        switch (opcion) {
            case 1:  coincide = movimientos[i].numeroAlquiler == numero; break;
            case 2:  coincide = strcmp(movimientos[i].dniCliente,
                                       texto) == 0;                      break;
            case 3:  coincide = strcmp(movimientos[i].patente,
                                       texto) == 0;                      break;
            default: coincide = (int)movimientos[i].tipo == numero;      break;
        }

        if (coincide) {
            if (encontrados == 0) {
                imprimirEncabezadoMovimientos();
            }

            imprimirFilaMovimiento(&movimientos[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron movimientos con ese criterio.\n");
    } else {
        printf("\nMovimientos encontrados: %d\n", encontrados);
    }
}

/* Saldo de un alquiler puntual o de todos los alquileres de un cliente */
static void consultarSaldo(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    char dni[12];
    int cantidad;
    int opcion;
    int numero = 0;
    int i;
    int encontrados = 0;

    mostrarTitulo("CONSULTA DE SALDOS");
    printf("1. Por numero de alquiler\n");
    printf("2. Por DNI del cliente\n");
    printf("0. Volver\n");

    opcion = leerOpcion("Seleccione una opcion: ");

    if (opcion == 0) {
        return;
    }

    if (opcion != 1 && opcion != 2) {
        printf("Error: opcion invalida.\n");
        return;
    }

    dni[0] = '\0';

    if (opcion == 1) {
        if (!pedirEntero("Numero de alquiler", 1, 999999, &numero)) {
            return;
        }
    } else if (!pedirDNI(dni)) {
        return;
    }

    cantidad = leerRegistros(ARCHIVO_ALQUILERES, alquileres,
                             sizeof(Alquiler), MAX_ALQUILERES);

    if (cantidad < 0) {
        printf("Error: no se pudo leer el archivo de alquileres.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if ((opcion == 1 && alquileres[i].numero == numero) ||
            (opcion == 2 && strcmp(alquileres[i].dniCliente, dni) == 0)) {
            if (encontrados == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron alquileres con ese dato.\n");
    }
}

static void actualizarArchivoCliente(void)
{
    Cliente cliente;
    char dni[12];

    mostrarTitulo("ACTUALIZAR ARCHIVO DE MOVIMIENTOS DE UN CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        return;
    }

    if (obtenerCliente(dni, &cliente) != 1) {
        printf("Error: no existe un cliente con el DNI %s.\n", dni);
        return;
    }

    if (generarHistorialCliente(dni)) {
        printf("Archivo %s/movimientos_%s.txt actualizado correctamente.\n",
               DIRECTORIO_DATOS, dni);
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
