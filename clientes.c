#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "clientes.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

/* =========================================================
   ACCESO A ARCHIVO
   ========================================================= */

static int leerClientes(Cliente clientes[], int *cantidad)
{
    int resultado = leerRegistros(ARCHIVO_CLIENTES, clientes,
                                  sizeof(Cliente), MAX_CLIENTES);

    if (resultado < 0) {
        printf("Error: no se pudo leer el archivo de clientes (%s).\n",
               ARCHIVO_CLIENTES);
        return 0;
    }

    *cantidad = resultado;
    return 1;
}

static int guardarClientes(const Cliente clientes[], int cantidad)
{
    if (!guardarRegistros(ARCHIVO_CLIENTES, clientes,
                          sizeof(Cliente), cantidad)) {
        printf("Error: no se pudo guardar el archivo de clientes.\n");
        return 0;
    }

    return 1;
}

static int posicionCliente(const Cliente clientes[], int cantidad,
                           const char dni[])
{
    int i;

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, dni) == 0) {
            return i;
        }
    }

    return -1;
}

int buscarClientePorDNI(const char dni[])
{
    Cliente clientes[MAX_CLIENTES];
    int cantidad;

    if (!leerClientes(clientes, &cantidad)) {
        return -1;
    }

    return posicionCliente(clientes, cantidad, dni);
}

int obtenerCliente(const char dni[], Cliente *cliente)
{
    Cliente clientes[MAX_CLIENTES];
    int cantidad;
    int posicion;

    if (!leerClientes(clientes, &cantidad)) {
        return -1;
    }

    posicion = posicionCliente(clientes, cantidad, dni);

    if (posicion < 0) {
        return 0;
    }

    *cliente = clientes[posicion];
    return 1;
}

/* =========================================================
   VALIDACIONES DE CAMPOS
   ========================================================= */

static int nombreValido(const char *texto)
{
    int i;
    int hayLetra = 0;

    for (i = 0; texto[i] != '\0'; i++) {
        if (isdigit((unsigned char)texto[i])) {
            return 0;
        }

        if (isalpha((unsigned char)texto[i])) {
            hayLetra = 1;
        }
    }

    return hayLetra;
}

static int telefonoValido(const char *texto)
{
    int i;
    int digitos = 0;

    for (i = 0; texto[i] != '\0'; i++) {
        if (isdigit((unsigned char)texto[i])) {
            digitos++;
        } else if (texto[i] != '+' && texto[i] != '-' &&
                   texto[i] != ' ' && texto[i] != '(' &&
                   texto[i] != ')') {
            return 0;
        }
    }

    return digitos >= 6;
}

static int emailValido(const char *texto)
{
    const char *arroba = strchr(texto, '@');
    const char *punto;

    if (strchr(texto, ' ') != NULL || arroba == NULL ||
        arroba == texto || strchr(arroba + 1, '@') != NULL) {
        return 0;
    }

    punto = strchr(arroba + 1, '.');

    return punto != NULL && punto > arroba + 1 && punto[1] != '\0';
}

/* Pide un dato del cliente segun el numero de campo. Devuelve 0 si cancela */
static int pedirDatoCliente(Cliente *cliente, int campo)
{
    switch (campo) {
        case 1:
            return pedirCampo("Apellido y nombre", cliente->nombre,
                              sizeof(cliente->nombre), nombreValido,
                              "el nombre debe tener letras y no puede "
                              "contener numeros.");

        case 2:
            return pedirCampo("Telefono", cliente->telefono,
                              sizeof(cliente->telefono), telefonoValido,
                              "telefono invalido (minimo 6 digitos; solo "
                              "numeros, espacios, + - y parentesis).");

        case 3:
            return pedirCampo("E-mail", cliente->email,
                              sizeof(cliente->email), emailValido,
                              "e-mail invalido (ejemplo: nombre@dominio.com).");

        case 4:
            return pedirCampo("Direccion", cliente->direccion,
                              sizeof(cliente->direccion), NULL, NULL);

        default:
            return 0;
    }
}

/* =========================================================
   PRESENTACION
   ========================================================= */

static void mostrarCliente(const Cliente *cliente)
{
    printf("\n----------------------------------------\n");
    printf("DNI:               %s\n", cliente->dni);
    printf("Apellido y nombre: %s\n", cliente->nombre);
    printf("Telefono:          %s\n", cliente->telefono);
    printf("E-mail:            %s\n", cliente->email);
    printf("Direccion:         %s\n", cliente->direccion);
    printf("Fecha de alta:     ");
    imprimirFecha(cliente->fechaAlta);
    printf("\n");
    printf("Estado:            %s\n", textoEstadoCliente(cliente->estado));
    printf("----------------------------------------\n");
}

/* =========================================================
   OPERACIONES
   ========================================================= */

void altaCliente(void)
{
    Cliente clientes[MAX_CLIENTES];
    Cliente nuevo;
    int cantidad;
    int posicion;
    int campo;

    mostrarTitulo("ALTA DE CLIENTE");

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    memset(&nuevo, 0, sizeof(Cliente));

    if (!pedirDNI(nuevo.dni)) {
        mostrarCancelado();
        return;
    }

    posicion = posicionCliente(clientes, cantidad, nuevo.dni);

    if (posicion >= 0) {
        if (clientes[posicion].estado == CLIENTE_ACTIVO) {
            printf("Error: ya existe un cliente activo con ese DNI.\n");
            mostrarCliente(&clientes[posicion]);
            return;
        }

        printf("El DNI corresponde a un cliente dado de baja.\n");
        mostrarCliente(&clientes[posicion]);

        if (confirmar("Desea reactivar al cliente")) {
            clientes[posicion].estado = CLIENTE_ACTIVO;

            if (guardarClientes(clientes, cantidad)) {
                printf("Cliente reactivado correctamente.\n");
            }
        } else {
            mostrarCancelado();
        }

        return;
    }

    if (cantidad >= MAX_CLIENTES) {
        printf("Error: se alcanzo el limite de %d clientes.\n", MAX_CLIENTES);
        return;
    }

    for (campo = 1; campo <= 4; campo++) {
        if (!pedirDatoCliente(&nuevo, campo)) {
            mostrarCancelado();
            return;
        }
    }

    nuevo.fechaAlta = fechaActual();
    nuevo.estado = CLIENTE_ACTIVO;

    clientes[cantidad] = nuevo;
    cantidad++;

    if (guardarClientes(clientes, cantidad)) {
        printf("\nCliente registrado correctamente.\n");
        mostrarCliente(&nuevo);
    }
}

void listarClientes(void)
{
    Cliente clientes[MAX_CLIENTES];
    int cantidad;
    int i;

    mostrarTitulo("LISTADO DE CLIENTES");

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    if (cantidad == 0) {
        printf("No hay clientes registrados.\n");
        return;
    }

    printf("%-9s %-28s %-15s %-26s %-10s %-8s\n",
           "DNI", "Apellido y nombre", "Telefono", "E-mail",
           "Alta", "Estado");
    printf("---------------------------------------------------------"
           "-------------------------------------\n");

    for (i = 0; i < cantidad; i++) {
        printf("%-9s %-28.28s %-15.15s %-26.26s ",
               clientes[i].dni, clientes[i].nombre,
               clientes[i].telefono, clientes[i].email);
        imprimirFecha(clientes[i].fechaAlta);
        printf(" %-8s\n", textoEstadoCliente(clientes[i].estado));
    }

    printf("\nTotal de clientes: %d\n", cantidad);
}

void consultarCliente(void)
{
    Cliente cliente;
    char dni[12];
    int resultado;

    mostrarTitulo("CONSULTA DE CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        return;
    }

    resultado = obtenerCliente(dni, &cliente);

    if (resultado < 0) {
        return;
    }

    if (resultado == 0) {
        printf("Error: no existe un cliente con el DNI %s.\n", dni);
        return;
    }

    mostrarCliente(&cliente);

    printf("\nHISTORIAL DE OPERACIONES\n");
    mostrarHistorialCliente(dni);
}

void modificarCliente(void)
{
    Cliente clientes[MAX_CLIENTES];
    Cliente editado;
    char dni[12];
    int cantidad;
    int posicion;
    int opcion;
    int campo;

    mostrarTitulo("MODIFICAR CLIENTE");

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        return;
    }

    posicion = posicionCliente(clientes, cantidad, dni);

    if (posicion < 0) {
        printf("Error: no existe un cliente con el DNI %s.\n", dni);
        return;
    }

    if (clientes[posicion].estado == CLIENTE_INACTIVO) {
        printf("Error: el cliente esta dado de baja. "
               "Reactivelo desde el alta de clientes.\n");
        return;
    }

    printf("\nDatos actuales:");
    mostrarCliente(&clientes[posicion]);

    printf("Que dato desea modificar?\n");
    printf("1. Apellido y nombre\n");
    printf("2. Telefono\n");
    printf("3. E-mail\n");
    printf("4. Direccion\n");
    printf("5. Todos los datos personales\n");
    printf("0. Cancelar\n");

    opcion = leerOpcion("Seleccione una opcion: ");

    if (opcion == 0) {
        mostrarCancelado();
        return;
    }

    if (opcion < 1 || opcion > 5) {
        printf("Error: opcion invalida.\n");
        return;
    }

    /* Se edita una copia: si el usuario cancela no se altera nada */
    editado = clientes[posicion];

    if (opcion == 5) {
        for (campo = 1; campo <= 4; campo++) {
            if (!pedirDatoCliente(&editado, campo)) {
                mostrarCancelado();
                return;
            }
        }
    } else if (!pedirDatoCliente(&editado, opcion)) {
        mostrarCancelado();
        return;
    }

    clientes[posicion] = editado;

    if (guardarClientes(clientes, cantidad)) {
        printf("\nCliente modificado correctamente.\n");
        mostrarCliente(&clientes[posicion]);
    }
}

void bajaLogicaCliente(void)
{
    Cliente clientes[MAX_CLIENTES];
    char dni[12];
    int cantidad;
    int posicion;
    int activo;

    mostrarTitulo("BAJA LOGICA DE CLIENTE");

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        return;
    }

    posicion = posicionCliente(clientes, cantidad, dni);

    if (posicion < 0) {
        printf("Error: no existe un cliente con el DNI %s.\n", dni);
        return;
    }

    if (clientes[posicion].estado == CLIENTE_INACTIVO) {
        printf("Error: el cliente ya se encuentra dado de baja.\n");
        return;
    }

    activo = clienteTieneAlquilerActivo(dni);

    if (activo < 0) {
        return;
    }

    if (activo > 0) {
        printf("Error: el cliente tiene alquileres activos. "
               "Registre la devolucion antes de darlo de baja.\n");
        return;
    }

    mostrarCliente(&clientes[posicion]);

    if (!confirmar("Confirma la baja del cliente")) {
        mostrarCancelado();
        return;
    }

    clientes[posicion].estado = CLIENTE_INACTIVO;

    if (guardarClientes(clientes, cantidad)) {
        printf("Cliente dado de baja correctamente.\n");
    }
}

void menuClientes(void)
{
    int opcion;

    do {
        printf("\n========================================\n");
        printf("             MENU CLIENTES\n");
        printf("========================================\n");
        printf("1. Alta de cliente\n");
        printf("2. Modificar cliente\n");
        printf("3. Consultar cliente (datos e historial)\n");
        printf("4. Listar clientes\n");
        printf("5. Baja logica de cliente\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1: altaCliente();        break;
            case 2: modificarCliente();   break;
            case 3: consultarCliente();   break;
            case 4: listarClientes();     break;
            case 5: bajaLogicaCliente();  break;
            case 0:                       break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
