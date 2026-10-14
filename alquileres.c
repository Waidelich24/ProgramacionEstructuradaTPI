#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "vehiculos.h"
#include "movimientos.h"

#define ARCHIVO_ALQUILERES "datos/alquileres.dat"
#define MAX_ALQUILERES 500
// (Añade esta función estática si no está disponible de forma global)
static void limpiarEntradaAlquileres(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Limpiar buffer */
    }
}

void menuAlquileres(void)
{
    int opcion;

    do {
        printf("\n--------------------------------\n");
        printf("      GESTION DE ALQUILERES     \n");
        printf("--------------------------------\n");
        printf("1. Realizar alquiler\n");
        printf("2. Registrar devolucion\n");
        printf("3. Listar alquileres\n");
        printf("0. Volver al menu principal\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarEntradaAlquileres();
            printf("Opcion invalida.\n");
            opcion = -1;
            continue;
        }

        limpiarEntradaAlquileres();

        switch (opcion) {
            case 1:
                realizarAlquiler();
                break;

            case 2:
                registrarDevolucion();
                break;

            case 3:
                listarAlquileres();
                break;

            case 0:
                printf("Volviendo al menu principal...\n");
                break;

            default:
                printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
/* Lee una linea de texto y elimina el salto de linea. */
static void leerTexto(const char *mensaje, char texto[], int capacidad)
{
    size_t largo;

    printf("%s", mensaje);
    if (fgets(texto, capacidad, stdin) == NULL) {
        texto[0] = '\0';
        return;
    }

    largo = strlen(texto);
    if (largo > 0 && texto[largo - 1] == '\n') {
        texto[largo - 1] = '\0';
    }
}

static int leerEntero(const char *mensaje, int *valor)
{
    char texto[50];
    char *fin;
    long numero;

    leerTexto(mensaje, texto, sizeof(texto));
    numero = strtol(texto, &fin, 10);

    while (*fin == ' ' || *fin == '\t') {
        fin++;
    }

    if (texto[0] == '\0' || *fin != '\0') {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

static int esBisiesto(int anio)
{
    return anio % 400 == 0 ||
           (anio % 4 == 0 && anio % 100 != 0);
}

static int fechaValida(Fecha fecha)
{
    int diasMes[12];

    diasMes[0] = 31; diasMes[1] = 28; diasMes[2] = 31;
    diasMes[3] = 30; diasMes[4] = 31; diasMes[5] = 30;
    diasMes[6] = 31; diasMes[7] = 31; diasMes[8] = 30;
    diasMes[9] = 31; diasMes[10] = 30; diasMes[11] = 31;

    if (fecha.anio < 1900 || fecha.anio > 2200 ||
        fecha.mes < 1 || fecha.mes > 12) {
        return 0;
    }

    if (esBisiesto(fecha.anio)) {
        diasMes[1] = 29;
    }

    return fecha.dia >= 1 && fecha.dia <= diasMes[fecha.mes - 1];
}

static int leerFecha(const char *mensaje, Fecha *fecha)
{
    printf("%s\n", mensaje);

    if (!leerEntero("  Dia: ", &fecha->dia) ||
        !leerEntero("  Mes: ", &fecha->mes) ||
        !leerEntero("  Anio: ", &fecha->anio) ||
        !fechaValida(*fecha)) {
        printf("Error: fecha invalida.\n");
        return 0;
    }

    return 1;
}

/* Convierte la fecha a una cantidad de dias para poder compararla. */
static long numeroDia(Fecha fecha)
{
    int anio;
    int mes;
    long total;

    total = 0;
    for (anio = 1; anio < fecha.anio; anio++) {
        total += esBisiesto(anio) ? 366L : 365L;
    }

    for (mes = 1; mes < fecha.mes; mes++) {
        if (mes == 2 && esBisiesto(fecha.anio)) {
            total += 29;
        } else {
            if (mes == 1 || mes == 3 || mes == 5 || mes == 7 ||
                mes == 8 || mes == 10 || mes == 12) {
                total += 31;
            } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
                total += 30;
            } else {
                total += 28;
            }
        }
    }

    total += fecha.dia - 1;
    return total;
}

static int diasEntre(Fecha inicio, Fecha fin)
{
    return (int)(numeroDia(fin) - numeroDia(inicio)) + 1;
}

static void mostrarFecha(Fecha fecha)
{
    printf("%02d/%02d/%04d", fecha.dia, fecha.mes, fecha.anio);
}

static int cargarAlquileres(Alquiler alquileres[])
{
    int cantidad;

    cantidad = leerRegistros(ARCHIVO_ALQUILERES, alquileres,
                             sizeof(Alquiler), MAX_ALQUILERES);
    if (cantidad < 0) {
        return 0;
    }
    return cantidad;
}

static int guardarAlquileres(Alquiler alquileres[], int cantidad)
{
    return guardarRegistros(ARCHIVO_ALQUILERES, alquileres,
                            sizeof(Alquiler), cantidad);
}

static int indiceAlquiler(Alquiler alquileres[], int cantidad, int numero)
{
    int i;

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].numero == numero) {
            return i;
        }
    }
    return -1;
}

static int nuevoNumero(Alquiler alquileres[], int cantidad)
{
    int i;
    int mayor;

    mayor = 0;
    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].numero > mayor) {
            mayor = alquileres[i].numero;
        }
    }
    return mayor + 1;
}


static int actualizarEstadoVehiculo(const char patente[],
                                    EstadoVehiculo estado)
{
    Vehiculo vehiculos[500];
    int cantidad;
    int i;

    cantidad = leerRegistros("datos/vehiculos.dat", vehiculos,
                             sizeof(Vehiculo), 500);
    if (cantidad < 0) {
        return 0;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, patente) == 0) {
            vehiculos[i].estado = estado;
            return guardarRegistros("datos/vehiculos.dat", vehiculos,
                                    sizeof(Vehiculo), cantidad);
        }
    }
    return 0;
}

static int cargarVehiculo(const char patente[], Vehiculo *vehiculo)
{
    Vehiculo vehiculos[500];
    int cantidad;
    int i;

    cantidad = leerRegistros("datos/vehiculos.dat", vehiculos,
                             sizeof(Vehiculo), 500);
    if (cantidad < 0) {
        return -1;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, patente) == 0) {
            *vehiculo = vehiculos[i];
            return i;
        }
    }
    return -1;
}

static int clienteActivo(const char dni[])
{
    Cliente clientes[500];
    int cantidad;
    int i;

    cantidad = leerRegistros("datos/clientes.dat", clientes,
                             sizeof(Cliente), 500);
    if (cantidad < 0) {
        return -1;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, dni) == 0) {
            return clientes[i].estado == CLIENTE_ACTIVO;
        }
    }
    return 0;
}

int buscarAlquilerPorNumero(int numero)
{
    Alquiler alquileres[MAX_ALQUILERES];
    int cantidad;

    cantidad = cargarAlquileres(alquileres);
    return indiceAlquiler(alquileres, cantidad, numero);
}


void realizarAlquiler(void)
{
    char dni[12];
    char patente[12];
    char confirmar[10];
    Vehiculo vehiculo;
    int estadoCliente;
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler nuevo;
    Movimiento movimiento;
    Fecha inicio;
    Fecha prevista;
    int cantidad;
    int dias;
    float importe;

    printf("\n--- REALIZAR ALQUILER ---\n");
    printf("*** PRUEBA: ENTRE A REALIZAR ALQUILER ***\n");
    fflush(stdout);

    leerTexto("DNI del cliente: ", dni, sizeof(dni));

    printf("DNI ingresado: [%s]\n", dni);
    fflush(stdout);

    if (buscarClientePorDNI(dni) < 0) {
        printf("Error: el cliente no existe.\n");
        return;
    }

    estadoCliente = clienteActivo(dni);
    if (estadoCliente < 0) {
        printf("Error: no se pudo leer el archivo de clientes.\n");
        return;
    }

    if (!estadoCliente) {
        printf("Error: el cliente esta inactivo.\n");
        return;
    }

    leerTexto("Patente del vehiculo: ", patente, sizeof(patente));
    if (buscarVehiculoPorPatente(patente) < 0 ||
        cargarVehiculo(patente, &vehiculo) < 0) {
        printf("Error: el vehiculo no existe o no se pudo leer.\n");
        return;
    }
    if (vehiculo.estado != VEHICULO_DISPONIBLE) {
        printf("Error: el vehiculo no esta disponible.\n");
        return;
    }

    if (!leerFecha("Fecha de inicio", &inicio) ||
        !leerFecha("Fecha prevista de devolucion", &prevista)) {
        return;
    }
    if (numeroDia(prevista) < numeroDia(inicio)) {
        printf("Error: la fecha prevista no puede ser anterior al inicio.\n");
        return;
    }

    dias = diasEntre(inicio, prevista);
    importe = vehiculo.precioDia * dias;

    printf("Dias: %d\n", dias);
    printf("Precio por dia: $%.2f\n", vehiculo.precioDia);
    printf("Importe total: $%.2f\n", importe);
    leerTexto("Confirmar alquiler? (S/N): ", confirmar, sizeof(confirmar));

    if (confirmar[0] != 'S' && confirmar[0] != 's') {
        printf("Operacion cancelada.\n");
        return;
    }

    cantidad = cargarAlquileres(alquileres);
    if (cantidad >= MAX_ALQUILERES) {
        printf("Error: se alcanzo el limite de alquileres.\n");
        return;
    }

    memset(&nuevo, 0, sizeof(nuevo));
    nuevo.numero = nuevoNumero(alquileres, cantidad);
    strcpy(nuevo.dniCliente, dni);
    strcpy(nuevo.patente, patente);
    nuevo.fechaInicio = inicio;
    nuevo.fechaPrevista = prevista;
    nuevo.cantidadDias = dias;
    nuevo.importeTotal = importe;
    nuevo.importePagado = 0.0f;
    nuevo.estado = ALQUILER_ACTIVO;

    alquileres[cantidad] = nuevo;
    if (!guardarAlquileres(alquileres, cantidad + 1)) {
        printf("Error: no se pudo guardar el alquiler.\n");
        return;
    }

    if (!actualizarEstadoVehiculo(patente, VEHICULO_ALQUILADO)) {
        /* Se intenta quitar el alquiler si no se pudo cambiar el vehiculo. */
        guardarAlquileres(alquileres, cantidad);
        printf("Error: no se pudo actualizar el estado del vehiculo.\n");
        return;
    }

    memset(&movimiento, 0, sizeof(movimiento));
    movimiento.numeroAlquiler = nuevo.numero;
    movimiento.tipo = MOV_ALQUILER;
    movimiento.monto = importe;
    movimiento.fecha = inicio;
    strcpy(movimiento.dniCliente, dni);
    strcpy(movimiento.patente, patente);
    registrarMovimiento(movimiento);

    printf("Alquiler registrado. Numero de alquiler: %d\n", nuevo.numero);
}

void registrarDevolucion(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler anterior;
    Movimiento movimiento;
    Fecha devolucion;
    Vehiculo vehiculo;
    int cantidad;
    int numero;
    int indice;
    int dias;
    float importeFinal;

    printf("\n--- REGISTRAR DEVOLUCION ---\n");

    if (!leerEntero("Numero de alquiler: ", &numero) || numero <= 0) {
        printf("Error: numero de alquiler invalido.\n");
        return;
    }

    cantidad = cargarAlquileres(alquileres);
    indice = indiceAlquiler(alquileres, cantidad, numero);
    if (indice < 0) {
        printf("Error: no existe ese alquiler.\n");
        return;
    }
    if (alquileres[indice].estado != ALQUILER_ACTIVO) {
        printf("Error: el alquiler no esta activo.\n");
        return;
    }

    if (!leerFecha("Fecha real de devolucion", &devolucion)) {
        return;
    }
    if (numeroDia(devolucion) < numeroDia(alquileres[indice].fechaInicio)) {
        printf("Error: la devolucion no puede ser anterior al inicio.\n");
        return;
    }

    if (buscarVehiculoPorPatente(alquileres[indice].patente) < 0 ||
        cargarVehiculo(alquileres[indice].patente, &vehiculo) < 0) {
        printf("Error: no se encontro el vehiculo asociado.\n");
        return;
    }

    dias = diasEntre(alquileres[indice].fechaInicio, devolucion);
    importeFinal = vehiculo.precioDia * dias;
    printf("Dias facturados: %d\n", dias);
    printf("Importe final: $%.2f\n", importeFinal);
    printf("Importe pagado: $%.2f\n", alquileres[indice].importePagado);

    if (alquileres[indice].importePagado > importeFinal) {
        printf("Error: el pago supera el importe final.\n");
        return;
    }

    anterior = alquileres[indice];
    alquileres[indice].fechaDevolucion = devolucion;
    alquileres[indice].cantidadDias = dias;
    alquileres[indice].importeTotal = importeFinal;
    alquileres[indice].estado = ALQUILER_FINALIZADO;

    if (!guardarAlquileres(alquileres, cantidad)) {
        printf("Error: no se pudo guardar la devolucion.\n");
        return;
    }

    if (!actualizarEstadoVehiculo(alquileres[indice].patente,
                                  VEHICULO_DISPONIBLE)) {
        alquileres[indice] = anterior;
        guardarAlquileres(alquileres, cantidad);
        printf("Error: no se pudo actualizar el vehiculo.\n");
        return;
    }

    memset(&movimiento, 0, sizeof(movimiento));
    movimiento.numeroAlquiler = numero;
    movimiento.tipo = MOV_DEVOLUCION;
    movimiento.monto = importeFinal;
    movimiento.fecha = devolucion;
    strcpy(movimiento.dniCliente, alquileres[indice].dniCliente);
    strcpy(movimiento.patente, alquileres[indice].patente);
    registrarMovimiento(movimiento);

    printf("Devolucion registrada correctamente.\n");
    printf("Saldo pendiente: $%.2f\n",
           importeFinal - alquileres[indice].importePagado);
}

void listarAlquileres(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    int cantidad;
    int i;
    const char *estado;

    printf("\n--- LISTA DE ALQUILERES ---\n");
    cantidad = cargarAlquileres(alquileres);

    if (cantidad == 0) {
        printf("No hay alquileres registrados o no se pudo leer el archivo.\n");
        return;
    }

    printf("%-6s %-12s %-10s %-12s %-12s %-12s %10s %10s\n",
           "Numero", "DNI", "Patente", "Inicio", "Prevista",
           "Estado", "Importe", "Pagado");
    printf("--------------------------------------------------------------------------\n");

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].estado == ALQUILER_ACTIVO) {
            estado = "Activo";
        } else if (alquileres[i].estado == ALQUILER_FINALIZADO) {
            estado = "Finalizado";
        } else if (alquileres[i].estado == ALQUILER_CANCELADO) {
            estado = "Cancelado";
        } else {
            estado = "Desconocido";
        }

        printf("%-6d %-12s %-10s ", alquileres[i].numero,
               alquileres[i].dniCliente, alquileres[i].patente);
        mostrarFecha(alquileres[i].fechaInicio);
        printf(" ");
        mostrarFecha(alquileres[i].fechaPrevista);
        printf(" %-12s %10.2f %10.2f\n", estado,
               alquileres[i].importeTotal, alquileres[i].importePagado);
    }
}
