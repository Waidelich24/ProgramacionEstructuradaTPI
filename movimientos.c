
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "movimientos.h"
#include "alquileres.h"
#include "archivos.h"

#define ARCHIVO_MOVIMIENTOS "datos/movimientos.dat"
#define ARCHIVO_ALQUILERES "datos/alquileres.dat"
#define MAX_MOVIMIENTOS 1000
#define MAX_ALQUILERES 500
#define TIPO_PAGO 1

/* Muestra mensajes destacados */
static void mostrarResultadoMov(const char mensaje[])
{
    printf("\n");
    printf("========================================\n");
    printf("  %s\n", mensaje);
    printf("========================================\n\n");
}

/* Limpia el buffer de entrada */
static void limpiarEntradaMovimientos(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

/* Lee un entero */
static int leerEnteroMov(const char *mensaje, int *valor)
{
    char texto[50];
    char *fin;
    long numero;

    printf("%s", mensaje);

    if (fgets(texto, sizeof(texto), stdin) == NULL) {
        return 0;
    }

    if (strchr(texto, '\n') == NULL) {
        limpiarEntradaMovimientos();
        return 0;
    }

    texto[strcspn(texto, "\n")] = '\0';

    if (texto[0] == '\0') {
        return 0;
    }

    numero = strtol(texto, &fin, 10);

    if (*fin != '\0' ||
        numero < -2147483647L - 1 ||
        numero > 2147483647L) {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

/* Lee un monto */
static int leerFlotanteMov(const char *mensaje, float *valor)
{
    char texto[50];
    char *fin;
    double numero;

    printf("%s", mensaje);

    if (fgets(texto, sizeof(texto), stdin) == NULL) {
        return 0;
    }

    if (strchr(texto, '\n') == NULL) {
        limpiarEntradaMovimientos();
        return 0;
    }

    texto[strcspn(texto, "\n")] = '\0';

    if (texto[0] == '\0') {
        return 0;
    }

    numero = strtod(texto, &fin);

    if (*fin != '\0' || numero <= 0 ||
        numero > 3.4e38 || numero != numero) {
        return 0;
    }

    *valor = (float)numero;

    if (*valor <= 0) {
        return 0;
    }

    return 1;
}

/* Obtiene la fecha actual */
static void obtenerFechaActual(Fecha *fecha)
{
    time_t ahora;
    struct tm *fechaActual;

    ahora = time(NULL);
    fechaActual = localtime(&ahora);

    if (fechaActual != NULL) {
        fecha->dia = fechaActual->tm_mday;
        fecha->mes = fechaActual->tm_mon + 1;
        fecha->anio = fechaActual->tm_year + 1900;
    } else {
        fecha->dia = 1;
        fecha->mes = 1;
        fecha->anio = 1900;
    }
}

/* Busca el siguiente numero de movimiento */
static int nuevoNumeroMovimiento(
    Movimiento movimientos[],
    int cantidad)
{
    int i;
    int mayor;

    mayor = 0;

    for (i = 0; i < cantidad; i++) {
        if (movimientos[i].numero > mayor) {
            mayor = movimientos[i].numero;
        }
    }

    return mayor + 1;
}

/* Genera el historial de movimientos de un cliente */
void generarHistorialCliente(const char dni[])
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;
    int i;
    int encontrados;
    char nombreArchivo[60];
    FILE *archivoTxt;

    cantidad = leerRegistros(
        ARCHIVO_MOVIMIENTOS,
        movimientos,
        sizeof(Movimiento),
        MAX_MOVIMIENTOS
    );

    if (cantidad < 0) {
        mostrarResultadoMov(
            "No se pudo leer el archivo de movimientos."
        );
        return;
    }

    sprintf(nombreArchivo, "datos/movimientos_%s.txt", dni);

    archivoTxt = fopen(nombreArchivo, "w");

    if (archivoTxt == NULL) {
        mostrarResultadoMov(
            "No se pudo crear el historial del cliente."
        );
        return;
    }

    fprintf(archivoTxt,
            "HISTORIAL DE MOVIMIENTOS - DNI: %s\n\n", dni);

    fprintf(archivoTxt,
            "%-8s %-8s %-10s %-12s %-12s %-12s\n",
            "NroMov", "NroAlq", "Tipo",
            "Monto", "Fecha", "Patente");

    fprintf(archivoTxt,
            "------------------------------------------------------------\n");

    encontrados = 0;

    for (i = 0; i < cantidad; i++) {
        if (strcmp(movimientos[i].dniCliente, dni) == 0) {
            fprintf(archivoTxt,
                    "%-8d %-8d %-10s $%-11.2f %02d/%02d/%04d %-12s\n",
                    movimientos[i].numero,
                    movimientos[i].numeroAlquiler,
                    movimientos[i].tipo == TIPO_PAGO
                        ? "Pago" : "Otro",
                    movimientos[i].monto,
                    movimientos[i].fecha.dia,
                    movimientos[i].fecha.mes,
                    movimientos[i].fecha.anio,
                    movimientos[i].patente);

            encontrados++;
        }
    }

    if (encontrados == 0) {
        fprintf(archivoTxt, "No hay movimientos registrados.\n");
    }

    fclose(archivoTxt);
}

/* Registra un movimiento en el archivo binario */
int registrarMovimiento(Movimiento movimiento)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;

    cantidad = leerRegistros(
        ARCHIVO_MOVIMIENTOS,
        movimientos,
        sizeof(Movimiento),
        MAX_MOVIMIENTOS
    );

    if (cantidad < 0) {
        cantidad = 0;
    }

    if (cantidad >= MAX_MOVIMIENTOS) {
        mostrarResultadoMov(
            "Se alcanzo el limite de movimientos."
        );
        return 0;
    }

    movimiento.numero = nuevoNumeroMovimiento(
        movimientos, cantidad
    );

    movimientos[cantidad] = movimiento;
    cantidad++;

    if (!guardarRegistros(
            ARCHIVO_MOVIMIENTOS,
            movimientos,
            sizeof(Movimiento),
            cantidad)) {
        mostrarResultadoMov(
            "Error al guardar el movimiento."
        );
        return 0;
    }

    generarHistorialCliente(movimiento.dniCliente);

    return 1;
}

/* Menu principal de movimientos */
void menuMovimientos(void)
{
    int opcion;
    int resultado;

    do {
        printf("\n========================================\n");
        printf("          GESTION DE MOVIMIENTOS\n");
        printf("========================================\n");
        printf("1. Registrar pago\n");
        printf("2. Consultar movimientos\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");
        printf("Seleccione una opcion: ");

        resultado = scanf("%d", &opcion);
        limpiarEntradaMovimientos();

        if (resultado != 1) {
            mostrarResultadoMov("Ingrese una opcion numerica.");
            opcion = -1;
            continue;
        }

        switch (opcion) {
            case 1:
                registrarPago();
                break;

            case 2:
                consultarMovimientos();
                break;

            case 0:
                mostrarResultadoMov(
                    "Volviendo al menu principal."
                );
                break;

            default:
                mostrarResultadoMov("Opcion invalida.");
        }

    } while (opcion != 0);
}

/* Registra un pago total o parcial */
void registrarPago(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Movimiento mov;
    int cantidadAlq;
    int numAlquiler;
    int indice;
    int i;
    float montoPago;
    float saldoPendiente;
    float importeAnterior;

    printf("\n========== REGISTRAR PAGO ==========\n");

    if (!leerEnteroMov(
            "Numero de alquiler: ", &numAlquiler)) {
        mostrarResultadoMov("Numero de alquiler invalido.");
        return;
    }

    if (numAlquiler <= 0) {
        mostrarResultadoMov(
            "El numero de alquiler debe ser positivo."
        );
        return;
    }

    cantidadAlq = leerRegistros(
        ARCHIVO_ALQUILERES,
        alquileres,
        sizeof(Alquiler),
        MAX_ALQUILERES
    );

    if (cantidadAlq <= 0) {
        mostrarResultadoMov(
            "No hay alquileres registrados o no se pudieron leer."
        );
        return;
    }

    indice = -1;

    for (i = 0; i < cantidadAlq; i++) {
        if (alquileres[i].numero == numAlquiler) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        mostrarResultadoMov("El alquiler no existe.");
        return;
    }

    saldoPendiente = alquileres[indice].importeTotal -
                     alquileres[indice].importePagado;

    printf("\n----------------------------------------\n");
    printf("Numero de alquiler: %d\n",
           alquileres[indice].numero);
    printf("Importe total:      $%.2f\n",
           alquileres[indice].importeTotal);
    printf("Importe pagado:     $%.2f\n",
           alquileres[indice].importePagado);
    printf("Saldo pendiente:    $%.2f\n",
           saldoPendiente);
    printf("----------------------------------------\n");

    if (saldoPendiente <= 0.01f) {
        mostrarResultadoMov(
            "El alquiler ya esta totalmente pagado."
        );
        return;
    }

    if (!leerFlotanteMov(
            "Ingrese el monto a pagar: ", &montoPago)) {
        mostrarResultadoMov("El monto ingresado no es valido.");
        return;
    }

    if (montoPago > saldoPendiente + 0.01f) {
        mostrarResultadoMov(
            "El pago supera el saldo pendiente."
        );
        return;
    }

    /* Evita que un pequeno redondeo deje un saldo negativo */
    if (montoPago > saldoPendiente) {
        montoPago = saldoPendiente;
    }

    importeAnterior = alquileres[indice].importePagado;
    alquileres[indice].importePagado += montoPago;

    /*
       Primero actualiza el alquiler.
       Si falla el registro del movimiento, intenta restaurarlo.
    */
    if (!guardarRegistros(
            ARCHIVO_ALQUILERES,
            alquileres,
            sizeof(Alquiler),
            cantidadAlq)) {
        mostrarResultadoMov(
            "No se pudo actualizar el alquiler."
        );
        return;
    }

    memset(&mov, 0, sizeof(Movimiento));

    mov.numeroAlquiler = alquileres[indice].numero;
    mov.tipo = TIPO_PAGO;
    mov.monto = montoPago;

    obtenerFechaActual(&mov.fecha);

    strcpy(mov.dniCliente, alquileres[indice].dniCliente);
    strcpy(mov.patente, alquileres[indice].patente);

    if (!registrarMovimiento(mov)) {
        alquileres[indice].importePagado = importeAnterior;

        if (!guardarRegistros(
                ARCHIVO_ALQUILERES,
                alquileres,
                sizeof(Alquiler),
                cantidadAlq)) {
            mostrarResultadoMov(
                "ATENCION: no se pudo restaurar el alquiler."
            );
        } else {
            mostrarResultadoMov(
                "El pago no se registro y se revirtio la actualizacion."
            );
        }

        return;
    }

    printf("Nuevo saldo pendiente: $%.2f\n",
           alquileres[indice].importeTotal -
           alquileres[indice].importePagado);

    mostrarResultadoMov("Pago registrado correctamente.");
}

/* Consulta todos los movimientos */
void consultarMovimientos(void)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;
    int i;

    printf("\n========== CONSULTAR MOVIMIENTOS ==========\n");

    cantidad = leerRegistros(
        ARCHIVO_MOVIMIENTOS,
        movimientos,
        sizeof(Movimiento),
        MAX_MOVIMIENTOS
    );

    if (cantidad < 0) {
        mostrarResultadoMov(
            "No se pudo leer el archivo de movimientos."
        );
        return;
    }

    if (cantidad == 0) {
        mostrarResultadoMov("No hay movimientos registrados.");
        return;
    }

    printf("%-8s %-8s %-8s %-12s %-12s %-12s\n",
           "NroMov", "NroAlq", "Tipo",
           "Monto", "Fecha", "DNI Cliente");

    printf("-------------------------------------------------------------------\n");

    for (i = 0; i < cantidad; i++) {
        printf("%-8d %-8d %-8s $%-11.2f %02d/%02d/%04d   %-12s\n",
               movimientos[i].numero,
               movimientos[i].numeroAlquiler,
               movimientos[i].tipo == TIPO_PAGO
                   ? "Pago" : "Otro",
               movimientos[i].monto,
               movimientos[i].fecha.dia,
               movimientos[i].fecha.mes,
               movimientos[i].fecha.anio,
               movimientos[i].dniCliente);
    }

    mostrarResultadoMov("Consulta de movimientos finalizada.");
}

