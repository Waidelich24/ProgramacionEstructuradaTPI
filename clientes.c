#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "clientes.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

/* Busca un cliente por DNI leyendo el archivo.
   Devuelve la posicion, -1 si no existe y -2 si no se pudo abrir el archivo */
static long buscarCliente(const char dni[], Cliente *encontrado)
{
    FILE *archivo = abrirLectura(ARCHIVO_CLIENTES);
    Cliente cliente;
    long pos = -1;
    long i = 0;

    if (archivo == NULL) {
        pos = -2;
    } else {
        while (pos == -1 && leerSiguiente(archivo, &cliente, sizeof(Cliente))) {
            if (strcmp(cliente.dni, dni) == 0) {
                pos = i;
                *encontrado = cliente;
            }
            i++;
        }
        cerrarArchivo(archivo);
    }

    return pos;
}

int buscarClientePorDNI(const char dni[])
{
    Cliente cliente;

    return (int)buscarCliente(dni, &cliente);
}

int obtenerCliente(const char dni[], Cliente *cliente)
{
    long pos = buscarCliente(dni, cliente);
    int resultado = 1;

    if (pos == -1) {
        resultado = 0;
    } else if (pos < -1) {
        printf("Error: no se pudo abrir el archivo de clientes.\n");
        resultado = -1;
    }

    return resultado;
}

/* ---------- Carga de datos ---------- */

static int pedirNombre(char nombre[])
{
    char texto[200];
    int i;
    int hayNumeros;
    int valido = 0;

    while (!valido && pedirTexto("Apellido y nombre", texto, 200)) {
        hayNumeros = 0;
        for (i = 0; texto[i] != '\0'; i++) {
            if (isdigit((unsigned char)texto[i])) {
                hayNumeros = 1;
            }
        }

        if (hayNumeros) {
            printf("Error: el nombre no puede tener numeros.\n");
        } else if (strlen(texto) >= 80) {
            printf("Error: el nombre es demasiado largo.\n");
        } else {
            strcpy(nombre, texto);
            valido = 1;
        }
    }

    return valido;
}

static int pedirTelefono(char telefono[])
{
    char texto[200];
    int i;
    int digitos;
    int caracteresMalos;
    int valido = 0;

    while (!valido && pedirTexto("Telefono", texto, 200)) {
        digitos = 0;
        caracteresMalos = 0;
        for (i = 0; texto[i] != '\0'; i++) {
            if (isdigit((unsigned char)texto[i])) {
                digitos++;
            } else if (texto[i] != '-' && texto[i] != ' ' && texto[i] != '+') {
                caracteresMalos = 1;
            }
        }

        if (caracteresMalos || digitos < 6 || strlen(texto) >= 24) {
            printf("Error: telefono invalido (minimo 6 numeros, puede llevar - o +).\n");
        } else {
            strcpy(telefono, texto);
            valido = 1;
        }
    }

    return valido;
}

static int pedirEmail(char email[])
{
    char texto[200];
    char *arroba;
    int valido = 0;

    while (!valido && pedirTexto("E-mail", texto, 200)) {
        arroba = strchr(texto, '@');

        if (arroba == NULL || strchr(arroba, '.') == NULL || strchr(texto, ' ') != NULL) {
            printf("Error: el e-mail debe tener @ y un punto (ej: nombre@mail.com).\n");
        } else if (strlen(texto) >= 80) {
            printf("Error: el e-mail es demasiado largo.\n");
        } else {
            strcpy(email, texto);
            valido = 1;
        }
    }

    return valido;
}

static int pedirDireccion(char direccion[])
{
    char texto[200];
    int valido = 0;

    while (!valido && pedirTexto("Direccion", texto, 200)) {
        if (strlen(texto) >= 120) {
            printf("Error: la direccion es demasiado larga.\n");
        } else {
            strcpy(direccion, texto);
            valido = 1;
        }
    }

    return valido;
}

/* Pide el dato numero "campo" del cliente (1 nombre, 2 telefono, 3 mail, 4 direccion) */
static int pedirDato(Cliente *cliente, int campo)
{
    int ok = 0;

    if (campo == 1) {
        ok = pedirNombre(cliente->nombre);
    } else if (campo == 2) {
        ok = pedirTelefono(cliente->telefono);
    } else if (campo == 3) {
        ok = pedirEmail(cliente->email);
    } else if (campo == 4) {
        ok = pedirDireccion(cliente->direccion);
    }

    return ok;
}

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
    printf("\nEstado:            %s\n", textoEstadoCliente(cliente->estado));
    printf("----------------------------------------\n");
}

/* ---------- Operaciones ---------- */

/* En las operaciones, "seguir" vale 1 mientras no haya habido ningun problema.
   Cuando algo falla se muestra un mensaje y seguir pasa a 0. */

void altaCliente(void)
{
    Cliente nuevo;
    Cliente existente;
    long pos;
    int seguir = 1;
    int campo;

    mostrarTitulo("ALTA DE CLIENTE");
    memset(&nuevo, 0, sizeof(Cliente));

    if (!pedirDNI(nuevo.dni)) {
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        pos = buscarCliente(nuevo.dni, &existente);

        if (pos < -1) {
            printf("Error: no se pudo abrir el archivo de clientes.\n");
            seguir = 0;
        } else if (pos >= 0 && existente.estado == CLIENTE_ACTIVO) {
            printf("Error: ya existe un cliente activo con ese DNI.\n");
            seguir = 0;
        } else if (pos >= 0) {
            /* el cliente existe pero esta dado de baja: no se crea uno nuevo */
            printf("Ese cliente esta dado de baja.\n");
            seguir = 0;

            if (confirmar("Desea reactivarlo")) {
                existente.estado = CLIENTE_ACTIVO;

                if (escribirRegistroEn(ARCHIVO_CLIENTES, &existente, sizeof(Cliente), pos)) {
                    printf("Cliente reactivado correctamente.\n");
                } else {
                    printf("Error: no se pudo guardar el cliente en el archivo.\n");
                }
            }
        }
    }

    for (campo = 1; seguir && campo <= 4; campo++) {
        if (!pedirDato(&nuevo, campo)) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    if (seguir) {
        nuevo.fechaAlta = fechaActual();
        nuevo.estado = CLIENTE_ACTIVO;

        if (agregarRegistro(ARCHIVO_CLIENTES, &nuevo, sizeof(Cliente))) {
            printf("\nCliente registrado correctamente.\n");
            mostrarCliente(&nuevo);
        } else {
            printf("Error: no se pudo guardar el cliente en el archivo.\n");
        }
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
        printf("Error: no se pudo abrir el archivo de clientes.\n");
    } else {
        printf("%-9s %-28s %-15s %-26s %-10s %-8s\n", "DNI", "Nombre",
               "Telefono", "E-mail", "Alta", "Estado");

        while (leerSiguiente(archivo, &cliente, sizeof(Cliente))) {
            printf("%-9s %-28.28s %-15.15s %-26.26s ", cliente.dni,
                   cliente.nombre, cliente.telefono, cliente.email);
            imprimirFecha(cliente.fechaAlta);
            printf(" %-8s\n", textoEstadoCliente(cliente.estado));
            cantidad++;
        }

        cerrarArchivo(archivo);
        printf("\nTotal de clientes: %d\n", cantidad);
    }
}

void consultarCliente(void)
{
    Cliente cliente;
    char dni[12];
    int resultado;

    mostrarTitulo("CONSULTA DE CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
    } else {
        resultado = obtenerCliente(dni, &cliente);

        if (resultado == 0) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
        } else if (resultado == 1) {
            mostrarCliente(&cliente);
            printf("\nHISTORIAL DE OPERACIONES\n");
            mostrarHistorialCliente(dni);
        }
    }
}

void modificarCliente(void)
{
    Cliente cliente;
    char dni[12];
    long pos = -1;
    int seguir = 1;
    int opcion = 0;
    int campo;

    mostrarTitulo("MODIFICAR CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        pos = buscarCliente(dni, &cliente);

        if (pos == -1) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
            seguir = 0;
        } else if (pos < -1) {
            printf("Error: no se pudo abrir el archivo de clientes.\n");
            seguir = 0;
        } else if (cliente.estado == CLIENTE_INACTIVO) {
            printf("Error: el cliente esta dado de baja y no se puede modificar.\n");
            seguir = 0;
        }
    }

    if (seguir) {
        mostrarCliente(&cliente);
        printf("Que dato desea modificar?\n");
        printf("1. Nombre\n2. Telefono\n3. E-mail\n4. Direccion\n5. Todos\n0. Cancelar\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 0) {
            mostrarCancelado();
            seguir = 0;
        } else if (opcion < 1 || opcion > 5) {
            printf("Error: opcion invalida.\n");
            seguir = 0;
        }
    }

    /* se modifica la copia; si el usuario cancela no se toca el archivo */
    if (seguir && opcion == 5) {
        for (campo = 1; seguir && campo <= 4; campo++) {
            if (!pedirDato(&cliente, campo)) {
                mostrarCancelado();
                seguir = 0;
            }
        }
    } else if (seguir && !pedirDato(&cliente, opcion)) {
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        if (escribirRegistroEn(ARCHIVO_CLIENTES, &cliente, sizeof(Cliente), pos)) {
            printf("\nCliente modificado correctamente.\n");
            mostrarCliente(&cliente);
        } else {
            printf("Error: no se pudo guardar el cliente en el archivo.\n");
        }
    }
}

void bajaLogicaCliente(void)
{
    Cliente cliente;
    char dni[12];
    long pos = -1;
    int seguir = 1;
    int activo;

    mostrarTitulo("BAJA DE CLIENTE");

    if (!pedirDNI(dni)) {
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        pos = buscarCliente(dni, &cliente);

        if (pos == -1) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
            seguir = 0;
        } else if (pos < -1) {
            printf("Error: no se pudo abrir el archivo de clientes.\n");
            seguir = 0;
        } else if (cliente.estado == CLIENTE_INACTIVO) {
            printf("Error: el cliente ya esta dado de baja.\n");
            seguir = 0;
        }
    }

    if (seguir) {
        activo = clienteTieneAlquilerActivo(dni);

        if (activo == 1) {
            printf("Error: el cliente tiene un alquiler activo. Primero registre la devolucion.\n");
        }

        if (activo != 0) {
            seguir = 0;
        }
    }

    if (seguir) {
        mostrarCliente(&cliente);

        if (!confirmar("Confirma la baja")) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    if (seguir) {
        cliente.estado = CLIENTE_INACTIVO;

        if (escribirRegistroEn(ARCHIVO_CLIENTES, &cliente, sizeof(Cliente), pos)) {
            printf("Cliente dado de baja correctamente.\n");
        } else {
            printf("Error: no se pudo guardar el cliente en el archivo.\n");
        }
    }
}

void menuClientes(void)
{
    int opcion;

    do {
        printf("\n========== MENU CLIENTES ==========\n");
        printf("1. Alta de cliente\n");
        printf("2. Modificar cliente\n");
        printf("3. Consultar cliente\n");
        printf("4. Listar clientes\n");
        printf("5. Baja de cliente\n");
        printf("0. Volver\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 1) {
            altaCliente();
        } else if (opcion == 2) {
            modificarCliente();
        } else if (opcion == 3) {
            consultarCliente();
        } else if (opcion == 4) {
            listarClientes();
        } else if (opcion == 5) {
            bajaLogicaCliente();
        } else if (opcion != 0) {
            printf("Opcion invalida.\n");
        }
    } while (opcion != 0);
}
