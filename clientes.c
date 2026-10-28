
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "clientes.h"


#define ARCHIVO_CLIENTES "datos/clientes.dat"
#define MAX_CLIENTES 500
#define CLIENTE_INACTIVO 0
#define CLIENTE_ACTIVO 1

/* Muestra un mensaje importante resaltado */
void mostrarResultado(const char mensaje[]) {
    printf("\n");
    printf("========================================\n");
    printf("  %s\n", mensaje);
    printf("========================================\n\n");
}

/* Limpia los caracteres que quedan en el teclado */
void limpiarEntrada(void) {
    int caracter;

    while ((caracter = getchar()) != '\n' && caracter != EOF) {
    }
}

/* Lee una linea de texto */
int leerLinea(char texto[], int tamanio) {
    size_t longitud;

    if (fgets(texto, tamanio, stdin) == NULL) {
        return 0;
    }

    longitud = strlen(texto);

    if (longitud > 0 && texto[longitud - 1] == '\n') {
        texto[longitud - 1] = '\0';
    } else {
        limpiarEntrada();
    }

    return 1;
}

/* Solicita y valida el DNI */
int pedirDNI(char dni[]) {
    int i;
    int valido;

    do {
        printf("Ingrese DNI (8 digitos): ");

        if (!leerLinea(dni, 12)) {
            return 0;
        }

        valido = strlen(dni) == 8;

        for (i = 0; dni[i] != '\0'; i++) {
            if (!isdigit((unsigned char)dni[i])) {
                valido = 0;
            }
        }

        if (!valido) {
            mostrarResultado("DNI invalido. Ingrese 8 digitos numericos.");
        }

    } while (!valido);

    return 1;
}

/* Muestra los datos de un cliente */
void mostrarCliente(Cliente cliente) {
    printf("\n----------------------------------------\n");
    printf("DNI:             %s\n", cliente.dni);
    printf("Nombre y apellido: %s\n", cliente.nombre);
    printf("Telefono:        %s\n", cliente.telefono);
    printf("Email:           %s\n", cliente.email);
    printf("Direccion:       %s\n", cliente.direccion);
    printf("Fecha de alta:   %02d/%02d/%04d\n",
           cliente.fechaAlta.dia,
           cliente.fechaAlta.mes,
           cliente.fechaAlta.anio);

    if (cliente.estado == CLIENTE_ACTIVO) {
        printf("Estado:          Activo\n");
    } else {
        printf("Estado:          Inactivo\n");
    }

    printf("----------------------------------------\n");
}

/* Valida una fecha */
int fechaValida(int dia, int mes, int anio) {
    int diasMes;

    if (anio < 1900 || mes < 1 || mes > 12 || dia < 1) {
        return 0;
    }

    if (mes == 2) {
        diasMes = 28;

        if ((anio % 4 == 0 && anio % 100 != 0) ||
            anio % 400 == 0) {
            diasMes = 29;
        }
    } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        diasMes = 30;
    } else {
        diasMes = 31;
    }

    return dia <= diasMes;
}

/* Solicita una fecha valida */
int pedirFecha(Fecha *fecha) {
    int resultado;

    do {
        printf("Dia: ");

        resultado = scanf("%d", &fecha->dia);
        limpiarEntrada();

        if (resultado != 1) {
            mostrarResultado("Debe ingresar un numero.");
            continue;
        }

        printf("Mes: ");

        resultado = scanf("%d", &fecha->mes);
        limpiarEntrada();

        if (resultado != 1) {
            mostrarResultado("Debe ingresar un numero.");
            continue;
        }

        printf("Anio: ");

        resultado = scanf("%d", &fecha->anio);
        limpiarEntrada();

        if (resultado != 1) {
            mostrarResultado("Debe ingresar un numero.");
            continue;
        }

        if (!fechaValida(fecha->dia, fecha->mes, fecha->anio)) {
            mostrarResultado("La fecha ingresada no es valida.");
        }

    } while (!fechaValida(fecha->dia, fecha->mes, fecha->anio));

    return 1;
}

/* Lee los clientes guardados en el archivo */
int leerClientes(Cliente clientes[], int *cantidad) {
    FILE *archivo;
    int resultado;

    resultado = leerRegistros(
        ARCHIVO_CLIENTES,
        clientes,
        sizeof(Cliente),
        MAX_CLIENTES
    );

    if (resultado >= 0) {
        *cantidad = resultado;
        return 1;
    }

    archivo = fopen(ARCHIVO_CLIENTES, "rb");

    if (archivo == NULL) {
        *cantidad = 0;
        return 1;
    }

    fclose(archivo);

    mostrarResultado("No se pudieron leer los clientes.");
    return 0;
}

/* Guarda todos los clientes */
int guardarClientes(Cliente clientes[], int cantidad) {
    if (!guardarRegistros(
            ARCHIVO_CLIENTES,
            clientes,
            sizeof(Cliente),
            cantidad)) {
        mostrarResultado("Error al guardar los clientes.");
        return 0;
    }

    return 1;
}

/* Busca un cliente por DNI y devuelve su posicion */
int buscarClientePorDNI(const char dni[]) {
    Cliente clientes[MAX_CLIENTES];
    int cantidad;
    int i;

    if (!leerClientes(clientes, &cantidad)) {
        return -1;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, dni) == 0) {
            return i;
        }
    }

    return -1;
}

/* Menu principal de clientes */
void menuClientes(void) {
    int opcion;
    int resultado;

    do {
        printf("\n========================================\n");
        printf("             MENU CLIENTES\n");
        printf("========================================\n");
        printf("1. Alta de cliente\n");
        printf("2. Modificar cliente\n");
        printf("3. Consultar cliente\n");
        printf("4. Listar clientes activos\n");
        printf("5. Baja logica de cliente\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");
        printf("Seleccione una opcion: ");

        resultado = scanf("%d", &opcion);
        limpiarEntrada();

        if (resultado != 1) {
            mostrarResultado("Ingrese una opcion numerica.");
            opcion = -1;
            continue;
        }

        switch (opcion) {
            case 1:
                altaCliente();
                break;

            case 2:
                modificarCliente();
                break;

            case 3:
                consultarCliente();
                break;

            case 4:
                listarClientes();
                break;

            case 5:
                bajaLogicaCliente();
                break;

            case 0:
                mostrarResultado("Volviendo al menu principal.");
                break;

            default:
                mostrarResultado("Opcion invalida.");
        }

    } while (opcion != 0);
}

/* Registra un nuevo cliente */
void altaCliente(void) {
    Cliente clientes[MAX_CLIENTES];
    Cliente nuevo;
    int cantidad;
    int i;

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    if (cantidad >= MAX_CLIENTES) {
        mostrarResultado("Se alcanzo el limite de clientes.");
        return;
    }

    memset(&nuevo, 0, sizeof(Cliente));

    printf("\n========== ALTA DE CLIENTE ==========\n");

    if (!pedirDNI(nuevo.dni)) {
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, nuevo.dni) == 0) {
            mostrarResultado("Ya existe un cliente con ese DNI.");
            return;
        }
    }

    printf("Nombre y apellido: ");
    if (!leerLinea(nuevo.nombre, sizeof(nuevo.nombre))) {
        mostrarResultado("No se pudo leer el nombre.");
        return;
    }

    printf("Telefono: ");
    if (!leerLinea(nuevo.telefono, sizeof(nuevo.telefono))) {
        mostrarResultado("No se pudo leer el telefono.");
        return;
    }

    printf("Email: ");
    if (!leerLinea(nuevo.email, sizeof(nuevo.email))) {
        mostrarResultado("No se pudo leer el email.");
        return;
    }

    printf("Direccion: ");
    if (!leerLinea(nuevo.direccion, sizeof(nuevo.direccion))) {
        mostrarResultado("No se pudo leer la direccion.");
        return;
    }

    printf("\nFecha de alta:\n");
    pedirFecha(&nuevo.fechaAlta);

    nuevo.estado = CLIENTE_ACTIVO;

    clientes[cantidad] = nuevo;
    cantidad++;

    if (guardarClientes(clientes, cantidad)) {
        mostrarResultado("Cliente registrado correctamente.");
        mostrarCliente(nuevo);
    }
}

/* Lista todos los clientes activos */
void listarClientes(void) {
    Cliente clientes[MAX_CLIENTES];
    int cantidad;
    int i;
    int encontrados;

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    printf("\n========== CLIENTES ACTIVOS ==========\n");

    encontrados = 0;

    for (i = 0; i < cantidad; i++) {
        if (clientes[i].estado == CLIENTE_ACTIVO) {
            mostrarCliente(clientes[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        mostrarResultado("No hay clientes activos registrados.");
    } else {
        mostrarResultado("Listado de clientes finalizado.");
    }
}

/* Consulta un cliente por DNI */
void consultarCliente(void) {
    Cliente clientes[MAX_CLIENTES];
    char dni[12];
    int cantidad;
    int i;

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    printf("\n========== CONSULTA DE CLIENTE ==========\n");

    if (!pedirDNI(dni)) {
        return;
    }

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, dni) == 0) {
            mostrarCliente(clientes[i]);
            mostrarResultado("Consulta finalizada.");
            return;
        }
    }

    mostrarResultado("No se encontro un cliente con ese DNI.");
}

/* Modifica los datos de un cliente */
void modificarCliente(void) {
    Cliente clientes[MAX_CLIENTES];
    char dni[12];
    int cantidad;
    int i;
    int posicion;
    int opcion;
    int resultado;

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    printf("\n========== MODIFICAR CLIENTE ==========\n");

    if (!pedirDNI(dni)) {
        return;
    }

    posicion = -1;

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, dni) == 0) {
            posicion = i;
            break;
        }
    }

    if (posicion == -1) {
        mostrarResultado("No se encontro un cliente con ese DNI.");
        return;
    }

    if (clientes[posicion].estado == CLIENTE_INACTIVO) {
        mostrarResultado("No se puede modificar un cliente inactivo.");
        return;
    }

    printf("\nDatos actuales del cliente:\n");
    mostrarCliente(clientes[posicion]);

    printf("\nQue dato desea modificar?\n");
    printf("1. Nombre y apellido\n");
    printf("2. Telefono\n");
    printf("3. Email\n");
    printf("4. Direccion\n");
    printf("5. Todos los datos personales\n");
    printf("0. Cancelar\n");
    printf("Seleccione una opcion: ");

    resultado = scanf("%d", &opcion);
    limpiarEntrada();

    if (resultado != 1) {
        mostrarResultado("Opcion invalida.");
        return;
    }

    switch (opcion) {
        case 1:
            printf("Nuevo nombre y apellido: ");
            if (!leerLinea(clientes[posicion].nombre,
                           sizeof(clientes[posicion].nombre))) {
                mostrarResultado("No se pudo leer el nombre.");
                return;
            }
            break;

        case 2:
            printf("Nuevo telefono: ");
            if (!leerLinea(clientes[posicion].telefono,
                           sizeof(clientes[posicion].telefono))) {
                mostrarResultado("No se pudo leer el telefono.");
                return;
            }
            break;

        case 3:
            printf("Nuevo email: ");
            if (!leerLinea(clientes[posicion].email,
                           sizeof(clientes[posicion].email))) {
                mostrarResultado("No se pudo leer el email.");
                return;
            }
            break;

        case 4:
            printf("Nueva direccion: ");
            if (!leerLinea(clientes[posicion].direccion,
                           sizeof(clientes[posicion].direccion))) {
                mostrarResultado("No se pudo leer la direccion.");
                return;
            }
            break;

        case 5:
            printf("Nuevo nombre y apellido: ");
            if (!leerLinea(clientes[posicion].nombre,
                           sizeof(clientes[posicion].nombre))) {
                mostrarResultado("No se pudo leer el nombre.");
                return;
            }

            printf("Nuevo telefono: ");
            if (!leerLinea(clientes[posicion].telefono,
                           sizeof(clientes[posicion].telefono))) {
                mostrarResultado("No se pudo leer el telefono.");
                return;
            }

            printf("Nuevo email: ");
            if (!leerLinea(clientes[posicion].email,
                           sizeof(clientes[posicion].email))) {
                mostrarResultado("No se pudo leer el email.");
                return;
            }

            printf("Nueva direccion: ");
            if (!leerLinea(clientes[posicion].direccion,
                           sizeof(clientes[posicion].direccion))) {
                mostrarResultado("No se pudo leer la direccion.");
                return;
            }
            break;

        case 0:
            mostrarResultado("Modificacion cancelada.");
            return;

        default:
            mostrarResultado("Opcion invalida.");
            return;
    }

    if (guardarClientes(clientes, cantidad)) {
        mostrarResultado("Cliente modificado correctamente.");
        mostrarCliente(clientes[posicion]);
    }
}

/* Realiza la baja logica de un cliente */
void bajaLogicaCliente(void) {
    Cliente clientes[MAX_CLIENTES];
    char dni[12];
    int cantidad;
    int i;
    int posicion;
    int confirmacion;
    int resultado;

    if (!leerClientes(clientes, &cantidad)) {
        return;
    }

    printf("\n========== BAJA DE CLIENTE ==========\n");

    if (!pedirDNI(dni)) {
        return;
    }

    posicion = -1;

    for (i = 0; i < cantidad; i++) {
        if (strcmp(clientes[i].dni, dni) == 0) {
            posicion = i;
            break;
        }
    }

    if (posicion == -1) {
        mostrarResultado("No se encontro un cliente con ese DNI.");
        return;
    }

    if (clientes[posicion].estado == CLIENTE_INACTIVO) {
        mostrarResultado("El cliente ya se encuentra dado de baja.");
        return;
    }

    printf("\nCliente seleccionado:\n");
    mostrarCliente(clientes[posicion]);

    printf("Confirma la baja? (1 = Si, 0 = No): ");

    resultado = scanf("%d", &confirmacion);
    limpiarEntrada();

    if (resultado != 1) {
        mostrarResultado("Opcion invalida. No se realizo la baja.");
        return;
    }

    if (confirmacion == 1) {
        clientes[posicion].estado = CLIENTE_INACTIVO;

        if (guardarClientes(clientes, cantidad)) {
            mostrarResultado("Cliente dado de baja correctamente.");
        }
    } else if (confirmacion == 0) {
        mostrarResultado("Baja cancelada. El cliente sigue activo.");
    } else {
        mostrarResultado("Opcion invalida. No se realizo la baja.");
    }
}

