#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movimientos.h"
#include "alquileres.h"
#include "archivos.h"

#define ARCHIVO_MOVIMIENTOS "datos/movimientos.dat"
#define MAX_MOVIMIENTOS 1000
static void limpiarEntradaMovimientos(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Limpiar buffer */
    }
}

void menuMovimientos(void)
{
    int opcion;

    do {
        printf("\n--------------------------------\n");
        printf("       GESTION DE MOVIMIENTOS   \n");
        printf("--------------------------------\n");
        printf("1. Registrar pago\n");
        printf("2. Consultar movimientos\n");
        printf("0. Volver al menu principal\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarEntradaMovimientos();
            printf("Opcion invalida.\n");
            opcion = -1;
            continue;
        }

        limpiarEntradaMovimientos();

        switch (opcion) {
            case 1:
                registrarPago();
                break;

            case 2:
                consultarMovimientos();
                break;

            case 0:
                printf("Volviendo al menu principal...\n");
                break;

            default:
                printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
/* Función auxiliar para leer enteros */
static int leerEnteroMov(const char *mensaje, int *valor)
{
    char texto[50];
    char *fin;
    long numero;

    printf("%s", mensaje);
    if (fgets(texto, sizeof(texto), stdin) == NULL) {
        return 0;
    }
    texto[strcspn(texto, "\n")] = '\0';
    numero = strtol(texto, &fin, 10);

    if (texto[0] == '\0' || *fin != '\0') {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

/* Función auxiliar para leer flotantes (montos) */
static int leerFlotanteMov(const char *mensaje, float *valor)
{
    char texto[50];
    char *fin;
    float numero;

    printf("%s", mensaje);
    if (fgets(texto, sizeof(texto), stdin) == NULL) {
        return 0;
    }
    texto[strcspn(texto, "\n")] = '\0';
    numero = strtof(texto, &fin);

    if (texto[0] == '\0' || *fin != '\0') {
        return 0;
    }

    *valor = numero;
    return 1;
}

/* Obtiene el siguiente número correlativo de movimiento */
static int nuevoNumeroMovimiento(Movimiento movimientos[], int cantidad)
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

/* Genera o actualiza el archivo de texto movimientos_DNI.txt con el historial del cliente[cite: 3] */
void generarHistorialCliente(const char dni[])
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidadMovs;
    int i;
    char nombreArchivo[50];
    FILE *archivoTxt;

    cantidadMovs = leerRegistros(ARCHIVO_MOVIMIENTOS, movimientos, sizeof(Movimiento), MAX_MOVIMIENTOS);
    if (cantidadMovs < 0) {
        cantidadMovs = 0;
    }

    sprintf(nombreArchivo, "datos/movimientos_%s.txt", dni);
    archivoTxt = fopen(nombreArchivo, "w");
    if (archivoTxt == NULL) {
        printf("Error: no se pudo crear el archivo de historial para el cliente DNI %s.\n", dni);
        return;
    }

    fprintf(archivoTxt, "=== HISTORIAL DE MOVIMIENTOS - CLIENTE DNI: %s ===\n", dni);
    fprintf(archivoTxt, "%-10s %-10s %-10s %-12s %-10s %-12s\n", 
            "NroMov", "NroAlq", "Tipo", "Monto", "Fecha", "Patente");
    fprintf(archivoTxt, "-------------------------------------------------------------\n");

    for (i = 0; i < cantidadMovs; i++) {
        if (strcmp(movimientos[i].dniCliente, dni) == 0) {
            fprintf(archivoTxt, "%-10d %-10d %-10d %-12.2f %02d/%02d/%04d %-12s\n",
                    movimientos[i].numero,
                    movimientos[i].numeroAlquiler,
                    movimientos[i].tipo,
                    movimientos[i].monto,
                    movimientos[i].fecha.dia,
                    movimientos[i].fecha.mes,
                    movimientos[i].fecha.anio,
                    movimientos[i].patente);
        }
    }

    fclose(archivoTxt);
}

/* Registra un movimiento en el archivo binario y actualiza el texto del cliente[cite: 3] */
void registrarMovimiento(Movimiento movimiento)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;

    cantidad = leerRegistros(ARCHIVO_MOVIMIENTOS, movimientos, sizeof(Movimiento), MAX_MOVIMIENTOS);
    if (cantidad < 0) {
        cantidad = 0;
    }

    if (cantidad >= MAX_MOVIMIENTOS) {
        printf("Error: se alcanzo el limite de movimientos.\n");
        return;
    }

    movimiento.numero = nuevoNumeroMovimiento(movimientos, cantidad);
    movimientos[cantidad] = movimiento;

    if (guardarRegistros(ARCHIVO_MOVIMIENTOS, movimientos, sizeof(Movimiento), cantidad + 1)) {
        generarHistorialCliente(movimiento.dniCliente);
    } else {
        printf("Error al guardar el movimiento en el archivo binario.\n");
    }
}

/* Registra un pago total o parcial sobre un alquiler[cite: 1, 2] */
void registrarPago(void)
{
    Alquiler alquileres[500];
    int cantidadAlq;
    int numAlquiler;
    int indice = -1;
    int i;
    float montoPago;
    float saldoPendiente;
    Movimiento mov;

    printf("\n--- REGISTRAR PAGO ---\n");

    if (!leerEnteroMov("Numero de alquiler: ", &numAlquiler)) {
        printf("Error: numero invalido.\n");
        return;
    }

    cantidadAlq = leerRegistros("datos/alquileres.dat", alquileres, sizeof(Alquiler), 500);
    if (cantidadAlq <= 0) {
        printf("Error al leer los alquileres.\n");
        return;
    }

    for (i = 0; i < cantidadAlq; i++) {
        if (alquileres[i].numero == numAlquiler) {
            indice = i;
            break;
        }
    }

    if (indice < 0) {
        printf("Error: el alquiler no existe[cite: 6].\n");
        return;
    }

    saldoPendiente = alquileres[indice].importeTotal - alquileres[indice].importePagado;
    printf("Importe Total: $%.2f\n", alquileres[indice].importeTotal);
    printf("Importe ya pagado: $%.2f\n", alquileres[indice].importePagado);
    printf("Saldo pendiente: $%.2f\n", saldoPendiente);

    if (saldoPendiente <= 0.01f) {
        printf("El alquiler ya se encuentra totalmente pagado.\n");
        return;
    }

    if (!leerFlotanteMov("Ingrese el monto a pagar: ", &montoPago)) {
        printf("Error: monto invalido.\n");
        return;
    }

    if (montoPago <= 0) {
        printf("Error: el monto debe ser mayor a cero[cite: 5].\n");
        return;
    }

    if (montoPago > saldoPendiente + 0.01f) {
        printf("Error: no se puede registrar un pago superior al saldo pendiente[cite: 5].\n");
        return;
    }

    alquileres[indice].importePagado += montoPago;

    if (!guardarRegistros("datos/alquileres.dat", alquileres, sizeof(Alquiler), cantidadAlq)) {
        printf("Error al actualizar el alquiler.\n");
        return;
    }

    memset(&mov, 0, sizeof(Movimiento));
    mov.numeroAlquiler = alquileres[indice].numero;
    mov.tipo = 1; // 1 para Pago
    mov.monto = montoPago;
    mov.fecha = alquileres[indice].fechaInicio; 
    strcpy(mov.dniCliente, alquileres[indice].dniCliente);
    strcpy(mov.patente, alquileres[indice].patente);

    registrarMovimiento(mov);

    printf("Pago registrado exitosamente. Nuevo saldo pendiente: $%.2f\n", 
           alquileres[indice].importeTotal - alquileres[indice].importePagado);
}

/* Consulta y lista los movimientos registrados en el sistema */
void consultarMovimientos(void)
{
    Movimiento movimientos[MAX_MOVIMIENTOS];
    int cantidad;
    int i;

    printf("\n--- CONSULTAR MOVIMIENTOS ---\n");
    cantidad = leerRegistros(ARCHIVO_MOVIMIENTOS, movimientos, sizeof(Movimiento), MAX_MOVIMIENTOS);

    if (cantidad <= 0) {
        printf("No hay movimientos registrados.\n");
        return;
    }

    printf("%-8s %-8s %-8s %-12s %-12s %-12s\n", 
           "NroMov", "NroAlq", "Tipo", "Monto", "Fecha", "DNI Cliente");
    printf("-------------------------------------------------------------------\n");

    for (i = 0; i < cantidad; i++) {
        printf("%-8d %-8d %-8d %-12.2f %02d/%02d/%04d   %-12s\n",
               movimientos[i].numero,
               movimientos[i].numeroAlquiler,
               movimientos[i].tipo,
               movimientos[i].monto,
               movimientos[i].fecha.dia,
               movimientos[i].fecha.mes,
               movimientos[i].fecha.anio,
               movimientos[i].dniCliente);
    }
}
