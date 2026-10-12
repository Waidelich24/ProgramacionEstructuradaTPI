
#include <stdio.h>
#include <string.h>

#include "clientes.h"
#include "archivos.h"

#define ARCHIVO_CLIENTES "datos/clientes.dat"
#define MAX_CLIENTES 500

int buscarClientePorDNI(const char dni[]) {
    Cliente clientes[MAX_CLIENTES];

    int cantidad = leerRegistros(
        ARCHIVO_CLIENTES,
        clientes,
        sizeof(Cliente),
        MAX_CLIENTES
    );

    if (cantidad < 0) {
        return -1;
    }

	int i;
	
	for (i = 0; i < cantidad; i++) {
	    if (strcmp(clientes[i].dni, dni) == 0) {
	        return i;
	    }
	}

    return -1;
}

void altaCliente(void) {
    printf("\n--- Alta de cliente ---\n");
    /* Pendiente de implementar */
}

void listarClientes(void) {
    printf("\n--- Lista de clientes ---\n");
    /* Pendiente de implementar */
}

void consultarCliente(void) {
    printf("\n--- Consultar cliente ---\n");
    /* Pendiente de implementar */
}

void modificarCliente(void) {
    printf("\n--- Modificar cliente ---\n");
    /* Pendiente de implementar */
}

void bajaLogicaCliente(void) {
    printf("\n--- Baja de cliente ---\n");
    /* Pendiente de implementar */
}
