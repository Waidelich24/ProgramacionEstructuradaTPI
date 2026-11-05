#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "clientes.h"
#include "alquileres.h"
#include "movimientos.h"
#include "archivos.h"
#include "utiles.h"

<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

int buscarClientePorDNI(const char dni[])
{
    Cliente cliente;

    return (int)buscarCliente(dni, &cliente);
}

CodigoError obtenerCliente(const char dni[], Cliente *cliente)
{
<<<<<<< HEAD
    long pos = buscarCliente(dni, cliente);
    int resultado = 1;

    if (pos == -1) {
        resultado = 0;
    } else if (pos < -1) {
        printf("Error: no se pudo abrir el archivo de clientes.\n");
        resultado = -1;
    }

    return resultado;
=======
    return errorPorPosicion(buscarCliente(dni, cliente));
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

/* ---------- Carga de datos ---------- */

static int pedirNombre(char nombre[])
{
    char texto[200];
    int i;
<<<<<<< HEAD
    int hayNumeros;
    int valido = 0;

    while (!valido && pedirTexto("Apellido y nombre", texto, 200)) {
        hayNumeros = 0;
        for (i = 0; texto[i] != '\0'; i++) {
            if (isdigit((unsigned char)texto[i])) {
                hayNumeros = 1;
            }
=======
    int hayLetra = 0;
    int sinNumeros = 1;

    for (i = 0; texto[i] != '\0'; i++) {
        if (isdigit((unsigned char)texto[i])) {
            sinNumeros = 0;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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

<<<<<<< HEAD
    return valido;
=======
    return hayLetra && sinNumeros;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

static int pedirTelefono(char telefono[])
{
    char texto[200];
    int i;
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

static int pedirEmail(char email[])
{
<<<<<<< HEAD
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
=======
    const char *arroba = strchr(texto, '@');
    const char *punto;
    int valido = 0;

    if (strchr(texto, ' ') == NULL && arroba != NULL && arroba != texto &&
        strchr(arroba + 1, '@') == NULL) {
        punto = strchr(arroba + 1, '.');
        valido = punto != NULL && punto > arroba + 1 && punto[1] != '\0';
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }

    return valido;
}

static int pedirDireccion(char direccion[])
{
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
<<<<<<< HEAD
    long pos;
    int seguir = 1;
=======
    CodigoError error = ERR_OK;
    long posicion;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    int campo;

    mostrarTitulo("ALTA DE CLIENTE");
    memset(&nuevo, 0, sizeof(Cliente));

    if (!pedirDNI(nuevo.dni)) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
<<<<<<< HEAD
        printf("Error: no se pudo abrir el archivo de clientes.\n");
    } else {
        printf("%-9s %-28s %-15s %-26s %-10s %-8s\n", "DNI", "Nombre",
               "Telefono", "E-mail", "Alta", "Estado");

        while (leerSiguiente(archivo, &cliente, sizeof(Cliente))) {
            printf("%-9s %-28.28s %-15.15s %-26.26s ", cliente.dni,
                   cliente.nombre, cliente.telefono, cliente.email);
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
            imprimirFecha(cliente.fechaAlta);
            printf(" %-8s\n", textoEstadoCliente(cliente.estado));
            cantidad++;
        }

        cerrarArchivo(archivo);
<<<<<<< HEAD
        printf("\nTotal de clientes: %d\n", cantidad);
=======

        if (cantidad == 0) {
            printf("No hay clientes registrados.\n");
        } else {
            printf("\nTotal de clientes: %d\n", cantidad);
        }
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
<<<<<<< HEAD
    } else {
        resultado = obtenerCliente(dni, &cliente);

        if (resultado == 0) {
            printf("Error: no existe un cliente con el DNI %s\n", dni);
        } else if (resultado == 1) {
            mostrarCliente(&cliente);
            printf("\nHISTORIAL DE OPERACIONES\n");
            mostrarHistorialCliente(dni);
        }
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void modificarCliente(void)
{
<<<<<<< HEAD
    Cliente cliente;
    char dni[12];
    long pos = -1;
    int seguir = 1;
=======
    Cliente editado;
    CodigoError error = ERR_OK;
    char dni[12];
    long posicion = -1;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    int opcion = 0;
    int campo;

    mostrarTitulo("MODIFICAR CLIENTE");

    if (!pedirDNI(dni)) {
<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void bajaLogicaCliente(void)
{
    Cliente cliente;
<<<<<<< HEAD
    char dni[12];
    long pos = -1;
    int seguir = 1;
    int activo;

    mostrarTitulo("BAJA DE CLIENTE");
=======
    CodigoError error = ERR_OK;
    char dni[12];
    long posicion = -1;
    int activo;

    mostrarTitulo("BAJA LOGICA DE CLIENTE");
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

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
<<<<<<< HEAD
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
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
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
