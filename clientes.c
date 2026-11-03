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

/* Busca por DNI leyendo el archivo registro por registro.
   Devuelve la posicion, -1 si no existe o -2 si no puede abrirse. */
static long buscarCliente(const char dni[], Cliente *salida)
{
    FILE *archivo = abrirLectura(ARCHIVO_CLIENTES);
    Cliente actual;
    long posicion = -1;
    long indice = 0;

    if (archivo == NULL) {
        posicion = -2;
    } else {
        while (posicion < 0 &&
               leerSiguiente(archivo, &actual, sizeof(Cliente))) {
            if (strcmp(actual.dni, dni) == 0) {
                posicion = indice;

                if (salida != NULL) {
                    *salida = actual;
                }
            }

            indice++;
        }

        cerrarArchivo(archivo);
    }

    return posicion;
}

int buscarClientePorDNI(const char dni[])
{
    Cliente cliente;

    return (int)buscarCliente(dni, &cliente);
}

CodigoError obtenerCliente(const char dni[], Cliente *cliente)
{
    return errorPorPosicion(buscarCliente(dni, cliente));
}

/* =========================================================
   VALIDACIONES DE CAMPOS
   ========================================================= */

static int nombreValido(const char *texto)
{
    int i;
    int hayLetra = 0;
    int sinNumeros = 1;

    for (i = 0; texto[i] != '\0'; i++) {
        if (isdigit((unsigned char)texto[i])) {
            sinNumeros = 0;
        }

        if (isalpha((unsigned char)texto[i])) {
            hayLetra = 1;
        }
    }

    return hayLetra && sinNumeros;
}

static int telefonoValido(const char *texto)
{
    int i;
    int digitos = 0;
    int caracteresValidos = 1;

    for (i = 0; texto[i] != '\0'; i++) {
        if (isdigit((unsigned char)texto[i])) {
            digitos++;
        } else if (texto[i] != '+' && texto[i] != '-' &&
                   texto[i] != ' ' && texto[i] != '(' &&
                   texto[i] != ')') {
            caracteresValidos = 0;
        }
    }

    return caracteresValidos && digitos >= 6;
}

static int emailValido(const char *texto)
{
    const char *arroba = strchr(texto, '@');
    const char *punto;
    int valido = 0;

    if (strchr(texto, ' ') == NULL && arroba != NULL && arroba != texto &&
        strchr(arroba + 1, '@') == NULL) {
        punto = strchr(arroba + 1, '.');
        valido = punto != NULL && punto > arroba + 1 && punto[1] != '\0';
    }

    return valido;
}

/* Pide un dato del cliente segun el numero de campo. Devuelve 0 si cancela */
static int pedirDatoCliente(Cliente *cliente, int campo)
{
    int ok = 0;

    switch (campo) {
        case 1:
            ok = pedirCampo("Apellido y nombre", cliente->nombre,
                            sizeof(cliente->nombre), nombreValido,
                            "el nombre debe tener letras y no puede "
                            "contener numeros.");
            break;

        case 2:
            ok = pedirCampo("Telefono", cliente->telefono,
                            sizeof(cliente->telefono), telefonoValido,
                            "telefono invalido (minimo 6 digitos; solo "
                            "numeros, espacios, + - y parentesis).");
            break;

        case 3:
            ok = pedirCampo("E-mail", cliente->email,
                            sizeof(cliente->email), emailValido,
                            "e-mail invalido (ejemplo: nombre@dominio.com).");
            break;

        case 4:
            ok = pedirCampo("Direccion", cliente->direccion,
                            sizeof(cliente->direccion), NULL, NULL);
            break;

        default:
            break;
    }

    return ok;
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

/* DNI ya registrado: informa y, si estaba de baja, ofrece reactivarlo.
   Devuelve ERR_DUPLICADO (no se crea un cliente nuevo) o ERR_CANCELADO. */
static CodigoError tratarClienteExistente(Cliente *existente, long posicion)
{
    CodigoError error = ERR_DUPLICADO;

    if (existente->estado == CLIENTE_ACTIVO) {
        printf("Error: ya existe un cliente activo con ese DNI.\n");
        mostrarCliente(existente);
    } else {
        printf("El DNI corresponde a un cliente dado de baja.\n");
        mostrarCliente(existente);

        if (confirmar("Desea reactivar al cliente")) {
            existente->estado = CLIENTE_ACTIVO;

            if (escribirRegistroEn(ARCHIVO_CLIENTES, existente,
                                   sizeof(Cliente), posicion) == ERR_OK) {
                printf("Cliente reactivado correctamente.\n");
            } else {
                mostrarError(ERR_ARCHIVO_ESCRIBIR, ARCHIVO_CLIENTES);
            }
        } else {
            error = ERR_CANCELADO;
        }
    }

    return error;
}

void altaCliente(void)
{
    Cliente nuevo;
    Cliente existente;
    CodigoError error = ERR_OK;
    long posicion;
    int campo;

    mostrarTitulo("ALTA DE CLIENTE");
    memset(&nuevo, 0, sizeof(Cliente));

    if (!pedirDNI(nuevo.dni)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        posicion = buscarCliente(nuevo.dni, &existente);

        if (posicion >= 0) {
            error = tratarClienteExistente(&existente, posicion);
        } else if (posicion < -1) {
            error = ERR_ARCHIVO_LEER;
            mostrarError(error, ARCHIVO_CLIENTES);
        }
    }

    for (campo = 1; error == ERR_OK && campo <= 4; campo++) {
        if (!pedirDatoCliente(&nuevo, campo)) {
            error = ERR_CANCELADO;
        }
    }

    if (error == ERR_OK) {
        nuevo.fechaAlta = fechaActual();
        nuevo.estado = CLIENTE_ACTIVO;
        error = agregarRegistro(ARCHIVO_CLIENTES, &nuevo, sizeof(Cliente));

        if (error == ERR_OK) {
            printf("\nCliente registrado correctamente.\n");
            mostrarCliente(&nuevo);
        } else {
            mostrarError(error, ARCHIVO_CLIENTES);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void listarClientes(void)
{
    FILE *archivo;
    Cliente cliente;
    int cantidad = 0;

    mostrarTitulo("LISTADO DE CLIENTES");
    archivo = abrirLectura(ARCHIVO_CLIENTES);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_CLIENTES);
    } else {
        while (leerSiguiente(archivo, &cliente, sizeof(Cliente))) {
            if (cantidad == 0) {
                printf("%-9s %-28s %-15s %-26s %-10s %-8s\n",
                       "DNI", "Apellido y nombre", "Telefono", "E-mail",
                       "Alta", "Estado");
                printf("-------------------------------------------------"
                       "-----------------------------------------------\n");
            }

            printf("%-9s %-28.28s %-15.15s %-26.26s ",
                   cliente.dni, cliente.nombre, cliente.telefono,
                   cliente.email);
            imprimirFecha(cliente.fechaAlta);
            printf(" %-8s\n", textoEstadoCliente(cliente.estado));
            cantidad++;
        }

        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay clientes registrados.\n");
        } else {
            printf("\nTotal de clientes: %d\n", cantidad);
        }
    }
}

void consultarCliente(void)
{
    Cliente cliente;
    CodigoError error = ERR_OK;
    char dni[12];

    mostrarTitulo("CONSULTA DE CLIENTE");

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
        mostrarCliente(&cliente);
        printf("\nHISTORIAL DE OPERACIONES\n");
        mostrarHistorialCliente(dni);
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void modificarCliente(void)
{
    Cliente editado;
    CodigoError error = ERR_OK;
    char dni[12];
    long posicion = -1;
    int opcion = 0;
    int campo;

    mostrarTitulo("MODIFICAR CLIENTE");

    if (!pedirDNI(dni)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        posicion = buscarCliente(dni, &editado);
        error = errorPorPosicion(posicion);

        if (error == ERR_NO_ENCONTRADO) {
            printf("Error: no existe un cliente con el DNI %s.\n", dni);
        } else if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_CLIENTES);
        } else if (editado.estado == CLIENTE_INACTIVO) {
            error = ERR_ESTADO_INVALIDO;
            printf("Error: el cliente esta dado de baja. "
                   "Reactivelo desde el alta de clientes.\n");
        }
    }

    if (error == ERR_OK) {
        printf("\nDatos actuales:");
        mostrarCliente(&editado);

        printf("Que dato desea modificar?\n");
        printf("1. Apellido y nombre\n");
        printf("2. Telefono\n");
        printf("3. E-mail\n");
        printf("4. Direccion\n");
        printf("5. Todos los datos personales\n");
        printf("0. Cancelar\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        if (opcion == 0) {
            error = ERR_CANCELADO;
        } else if (opcion < 1 || opcion > 5) {
            error = ERR_DATO_INVALIDO;
            printf("Error: opcion invalida.\n");
        }
    }

    /* Se edita la copia en memoria: si se cancela, el archivo no cambia */
    if (error == ERR_OK && opcion == 5) {
        for (campo = 1; error == ERR_OK && campo <= 4; campo++) {
            if (!pedirDatoCliente(&editado, campo)) {
                error = ERR_CANCELADO;
            }
        }
    } else if (error == ERR_OK && !pedirDatoCliente(&editado, opcion)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        error = escribirRegistroEn(ARCHIVO_CLIENTES, &editado,
                                   sizeof(Cliente), posicion);

        if (error == ERR_OK) {
            printf("\nCliente modificado correctamente.\n");
            mostrarCliente(&editado);
        } else {
            mostrarError(error, ARCHIVO_CLIENTES);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void bajaLogicaCliente(void)
{
    Cliente cliente;
    CodigoError error = ERR_OK;
    char dni[12];
    long posicion = -1;
    int activo;

    mostrarTitulo("BAJA LOGICA DE CLIENTE");

    if (!pedirDNI(dni)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        posicion = buscarCliente(dni, &cliente);
        error = errorPorPosicion(posicion);

        if (error == ERR_NO_ENCONTRADO) {
            printf("Error: no existe un cliente con el DNI %s.\n", dni);
        } else if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_CLIENTES);
        } else if (cliente.estado == CLIENTE_INACTIVO) {
            error = ERR_ESTADO_INVALIDO;
            printf("Error: el cliente ya se encuentra dado de baja.\n");
        }
    }

    if (error == ERR_OK) {
        activo = clienteTieneAlquilerActivo(dni);

        if (activo < 0) {
            error = ERR_ARCHIVO_LEER;
        } else if (activo > 0) {
            error = ERR_ESTADO_INVALIDO;
            printf("Error: el cliente tiene alquileres activos. "
                   "Registre la devolucion antes de darlo de baja.\n");
        }
    }

    if (error == ERR_OK) {
        mostrarCliente(&cliente);

        if (!confirmar("Confirma la baja del cliente")) {
            error = ERR_CANCELADO;
        }
    }

    if (error == ERR_OK) {
        cliente.estado = CLIENTE_INACTIVO;
        error = escribirRegistroEn(ARCHIVO_CLIENTES, &cliente,
                                   sizeof(Cliente), posicion);

        if (error == ERR_OK) {
            printf("Cliente dado de baja correctamente.\n");
        } else {
            mostrarError(error, ARCHIVO_CLIENTES);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
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
