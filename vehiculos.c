#include <stdio.h>
#include <string.h>

#include "vehiculos.h"
#include "archivos.h"
#include "utiles.h"

#define ANIO_VEHICULO_MINIMO 1950
#define CAPACIDAD_MAXIMA     60

/* =========================================================
   ACCESO A ARCHIVO
   ========================================================= */

static int leerVehiculos(Vehiculo vehiculos[], int *cantidad)
{
    int resultado = leerRegistros(ARCHIVO_VEHICULOS, vehiculos,
                                  sizeof(Vehiculo), MAX_VEHICULOS);

    if (resultado < 0) {
        printf("Error: no se pudo leer el archivo de vehiculos (%s).\n",
               ARCHIVO_VEHICULOS);
        return 0;
    }

    *cantidad = resultado;
    return 1;
}

static int guardarVehiculos(const Vehiculo vehiculos[], int cantidad)
{
    if (!guardarRegistros(ARCHIVO_VEHICULOS, vehiculos,
                          sizeof(Vehiculo), cantidad)) {
        printf("Error: no se pudo guardar el archivo de vehiculos.\n");
        return 0;
    }

    return 1;
}

static int posicionVehiculo(const Vehiculo vehiculos[], int cantidad,
                            const char patente[])
{
    int i;

    for (i = 0; i < cantidad; i++) {
        if (strcmp(vehiculos[i].patente, patente) == 0) {
            return i;
        }
    }

    return -1;
}

int buscarVehiculoPorPatente(const char patente[])
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return -1;
    }

    return posicionVehiculo(vehiculos, cantidad, patente);
}

int obtenerVehiculo(const char patente[], Vehiculo *vehiculo)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;
    int posicion;

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return -1;
    }

    posicion = posicionVehiculo(vehiculos, cantidad, patente);

    if (posicion < 0) {
        return 0;
    }

    *vehiculo = vehiculos[posicion];
    return 1;
}

int actualizarEstadoVehiculo(const char patente[], EstadoVehiculo estado)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;
    int posicion;

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return 0;
    }

    posicion = posicionVehiculo(vehiculos, cantidad, patente);

    if (posicion < 0) {
        printf("Error: no existe el vehiculo %s.\n", patente);
        return 0;
    }

    vehiculos[posicion].estado = estado;
    return guardarVehiculos(vehiculos, cantidad);
}

/* =========================================================
   PRESENTACION
   ========================================================= */

static void imprimirEncabezadoVehiculos(void)
{
    printf("%-9s %-14s %-16s %-5s %-12s %-4s %12s  %-16s\n",
           "Patente", "Marca", "Modelo", "Anio", "Tipo", "Cap.",
           "Precio/dia", "Estado");
    printf("---------------------------------------------------------"
           "-------------------------------------\n");
}

static void imprimirFilaVehiculo(const Vehiculo *v)
{
    printf("%-9s %-14.14s %-16.16s %-5d %-12.12s %-4d %12.2f  %-16s\n",
           v->patente, v->marca, v->modelo, v->anio, v->tipo,
           v->capacidad, v->precioDia, textoEstadoVehiculo(v->estado));
}

static void mostrarVehiculo(const Vehiculo *v)
{
    printf("\n----------------------------------------\n");
    printf("Patente:        %s\n", v->patente);
    printf("Marca:          %s\n", v->marca);
    printf("Modelo:         %s\n", v->modelo);
    printf("Anio:           %d\n", v->anio);
    printf("Tipo:           %s\n", v->tipo);
    printf("Capacidad:      %d\n", v->capacidad);
    printf("Precio por dia: $%.2f\n", v->precioDia);
    printf("Estado:         %s\n", textoEstadoVehiculo(v->estado));
    printf("----------------------------------------\n");
}

/* =========================================================
   CARGA DE DATOS
   ========================================================= */

/* Pide un dato del vehiculo segun el numero de campo. Devuelve 0 si cancela */
static int pedirDatoVehiculo(Vehiculo *v, int campo)
{
    int numero;
    float precio;

    switch (campo) {
        case 1:
            return pedirCampo("Marca", v->marca, sizeof(v->marca),
                              NULL, NULL);

        case 2:
            return pedirCampo("Modelo", v->modelo, sizeof(v->modelo),
                              NULL, NULL);

        case 3:
            if (!pedirEntero("Anio de fabricacion", ANIO_VEHICULO_MINIMO,
                             fechaActual().anio + 1, &numero)) {
                return 0;
            }

            v->anio = numero;
            return 1;

        case 4:
            return pedirCampo("Tipo (sedan, pick-up, utilitario...)",
                              v->tipo, sizeof(v->tipo), NULL, NULL);

        case 5:
            if (!pedirEntero("Capacidad de pasajeros", 1, CAPACIDAD_MAXIMA,
                             &numero)) {
                return 0;
            }

            v->capacidad = numero;
            return 1;

        case 6:
            if (!pedirMonto("Precio por dia", 0.0f, &precio)) {
                return 0;
            }

            v->precioDia = precio;
            return 1;

        default:
            return 0;
    }
}

/* Cambia el estado manual (mantenimiento / baja / disponible) */
static int pedirNuevoEstado(Vehiculo *v)
{
    int opcion;

    if (v->estado == VEHICULO_ALQUILADO) {
        printf("Error: el vehiculo esta alquilado. Registre primero la "
               "devolucion para poder cambiar su estado.\n");
        return 0;
    }

    printf("\nSeleccione el nuevo estado:\n");
    printf("1. Disponible\n");
    printf("2. En mantenimiento\n");
    printf("3. Baja\n");
    printf("0. Cancelar\n");

    opcion = leerOpcion("Opcion: ");

    switch (opcion) {
        case 1: v->estado = VEHICULO_DISPONIBLE;    return 1;
        case 2: v->estado = VEHICULO_MANTENIMIENTO; return 1;
        case 3: v->estado = VEHICULO_BAJA;          return 1;
        case 0: return 0;
        default:
            printf("Error: opcion invalida.\n");
            return 0;
    }
}

/* =========================================================
   OPERACIONES
   ========================================================= */

void altaVehiculo(void)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    Vehiculo nuevo;
    int cantidad;
    int campo;

    mostrarTitulo("ALTA DE VEHICULO");

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return;
    }

    if (cantidad >= MAX_VEHICULOS) {
        printf("Error: se alcanzo el limite de %d vehiculos.\n",
               MAX_VEHICULOS);
        return;
    }

    memset(&nuevo, 0, sizeof(Vehiculo));

    if (!pedirPatente(nuevo.patente)) {
        mostrarCancelado();
        return;
    }

    if (posicionVehiculo(vehiculos, cantidad, nuevo.patente) >= 0) {
        printf("Error: ya existe un vehiculo con la patente %s.\n",
               nuevo.patente);
        return;
    }

    for (campo = 1; campo <= 6; campo++) {
        if (!pedirDatoVehiculo(&nuevo, campo)) {
            mostrarCancelado();
            return;
        }
    }

    nuevo.estado = VEHICULO_DISPONIBLE;

    if (!agregarRegistro(ARCHIVO_VEHICULOS, &nuevo, sizeof(Vehiculo))) {
        printf("Error: no se pudo guardar el vehiculo.\n");
        return;
    }

    printf("\nVehiculo registrado correctamente.\n");
    mostrarVehiculo(&nuevo);
}

void listarVehiculos(void)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;
    int i;

    mostrarTitulo("LISTADO DE VEHICULOS");

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return;
    }

    if (cantidad == 0) {
        printf("No hay vehiculos registrados.\n");
        return;
    }

    imprimirEncabezadoVehiculos();

    for (i = 0; i < cantidad; i++) {
        imprimirFilaVehiculo(&vehiculos[i]);
    }

    printf("\nTotal de vehiculos: %d\n", cantidad);
}

int listarVehiculosDisponibles(void)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad;
    int i;
    int disponibles = 0;

    mostrarTitulo("VEHICULOS DISPONIBLES");

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return -1;
    }

    for (i = 0; i < cantidad; i++) {
        if (vehiculos[i].estado == VEHICULO_DISPONIBLE) {
            if (disponibles == 0) {
                imprimirEncabezadoVehiculos();
            }

            imprimirFilaVehiculo(&vehiculos[i]);
            disponibles++;
        }
    }

    if (disponibles == 0) {
        printf("No hay vehiculos disponibles en este momento.\n");
    } else {
        printf("\nVehiculos disponibles: %d\n", disponibles);
    }

    return disponibles;
}

void consultarVehiculo(void)
{
    Vehiculo vehiculo;
    char patente[12];
    int resultado;

    mostrarTitulo("BUSCAR VEHICULO");

    if (!pedirPatente(patente)) {
        mostrarCancelado();
        return;
    }

    resultado = obtenerVehiculo(patente, &vehiculo);

    if (resultado < 0) {
        return;
    }

    if (resultado == 0) {
        printf("Error: no existe un vehiculo con la patente %s.\n", patente);
        return;
    }

    mostrarVehiculo(&vehiculo);
}

void modificarVehiculo(void)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    Vehiculo editado;
    char patente[12];
    int cantidad;
    int posicion;
    int opcion;
    int campo;

    mostrarTitulo("MODIFICAR VEHICULO");

    if (!leerVehiculos(vehiculos, &cantidad)) {
        return;
    }

    if (!pedirPatente(patente)) {
        mostrarCancelado();
        return;
    }

    posicion = posicionVehiculo(vehiculos, cantidad, patente);

    if (posicion < 0) {
        printf("Error: no existe un vehiculo con la patente %s.\n", patente);
        return;
    }

    printf("\nDatos actuales:");
    mostrarVehiculo(&vehiculos[posicion]);

    printf("Que desea modificar?\n");
    printf("1. Marca\n");
    printf("2. Modelo\n");
    printf("3. Anio\n");
    printf("4. Tipo de vehiculo\n");
    printf("5. Capacidad\n");
    printf("6. Precio por dia\n");
    printf("7. Estado\n");
    printf("8. Todos los datos (excepto el estado)\n");
    printf("0. Cancelar\n");

    opcion = leerOpcion("Seleccione una opcion: ");

    if (opcion == 0) {
        mostrarCancelado();
        return;
    }

    if (opcion < 1 || opcion > 8) {
        printf("Error: opcion invalida.\n");
        return;
    }

    /* Se edita una copia: si el usuario cancela no se altera nada */
    editado = vehiculos[posicion];

    if (opcion == 7) {
        if (!pedirNuevoEstado(&editado)) {
            mostrarCancelado();
            return;
        }
    } else if (opcion == 8) {
        for (campo = 1; campo <= 6; campo++) {
            if (!pedirDatoVehiculo(&editado, campo)) {
                mostrarCancelado();
                return;
            }
        }
    } else if (!pedirDatoVehiculo(&editado, opcion)) {
        mostrarCancelado();
        return;
    }

    vehiculos[posicion] = editado;

    if (guardarVehiculos(vehiculos, cantidad)) {
        printf("\nVehiculo modificado correctamente.\n");
        mostrarVehiculo(&vehiculos[posicion]);
    }
}

void menuVehiculos(void)
{
    int opcion;

    do {
        printf("\n========================================\n");
        printf("            MENU DE VEHICULOS\n");
        printf("========================================\n");
        printf("1. Alta de vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Buscar vehiculo por patente\n");
        printf("4. Listar todos los vehiculos\n");
        printf("5. Listar vehiculos disponibles\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1: altaVehiculo();                break;
            case 2: modificarVehiculo();           break;
            case 3: consultarVehiculo();           break;
            case 4: listarVehiculos();             break;
            case 5: listarVehiculosDisponibles();  break;
            case 0:                                break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
