
#include <stdio.h>
#include <string.h>

#include "clientes.h"
#include "archivos.h"

#define ARCHIVO_CLIENTES "datos/clientes.dat"
#define MAX_CLIENTES 500
#define CLIENTE_INACTIVO 0
#define CLIENTE_ACTIVO 1

//Funciones Auxiliares (Solo se usan dentro de este archivo)

static void leerlinea(const char *mensaje, char *buffer, size_t tam) {
    //Mostrar el mensaje 
    printf("%s", mensaje);

    //Leer la línea de entrada del usuario
    fgets(buffer, tam, stdin);
    //Eliminar el salto de línea al final de la cadena
    buffer[strcspn(buffer, "\n")] = 0; 
}

static void pedirDNI(char dni[]) {
    char buffer[12];
    int largo;

    do {
        leerLinea("Ingrese DNI (sin puntos ni guiones): ", dni, 12);
        if(strlen(dni) != 8) {
            printf("Error: El DNI debe tener 8 caracteres. Intente nuevamente.\n");
        }
    } while(strlen(dni) != 8);
}

static void mostrarCliente(const Cliente *c){
    printf("DNI: %s", c->dni);
    printf("\nNombre y Apellido: %s", c->nombre);
    printf("\nTelefono: %s", c->telefono);
    printf("\nEmail: %s", c->email);
    printf("\nDireccion: %s", c->direccion);
    printf("\nFecha de alta: %02d/%02d/%04d", c->fechaAlta.dia, c->fechaAlta.mes, c->fechaAlta.anio);
    if(c->estado == CLIENTE_ACTIVO) {
        printf("\nEstado: Activo");
    } else {
        printf("\nEstado: Inactivo");
    }
}

//---------------------------------------------------------


int buscarClientePorDNI(const char dni[]) {
    Cliente clientes[MAX_CLIENTES];
    int i;

    int cantidad = leerRegistros(
        ARCHIVO_CLIENTES, //Ruta
        clientes, //Registros
        sizeof(Cliente), //TamRegistro
        MAX_CLIENTES //Capacidad
    );

    if (cantidad < 0) {
        return -1;
    }

	for (i = 0; i < cantidad; i++) {
	    if (strcmp(clientes[i].dni, dni) == 0) {
	        return i;
	    }
	}

    return -1;
}

void altaCliente(void) {
    Cliente c;
    char buf[256];

    printf("\n--- Alta de cliente ---\n");
    //Llenar con ceros la estructura Cliente
    memset(&c, 0, sizeof(Cliente));

    //Comprobar que el DNI no exista en el archivo
    pedirDNI(c.dni);
    if(buscarClientePorDNI(c.dni) >= 0) {
        printf("Error: Ya existe un cliente con ese DNI.\n");
        return;
    }

    //Pedir datos del cliente
    leerLinea("Nombre y Apellido: ", c.nombre, sizeof(c.nombre));
    leerLinea("Telefono: ", c.telefono, sizeof(c.telefono));
    leerLinea("Email: ", c.email, sizeof(c.email));
    leerLinea("Direccion: ", c.direccion, sizeof(c.direccion));

    //Pedir fecha con do para evitar errores de ingreso
    do{
        c.fechaAlta.dia=0;
        c.fechaAlta.mes=0;
        c.fechaAlta.anio=0;
        leerFecha("Fecha de alta (dd/mm/aaaa): ", &c.fechaAlta);
        if(c.fechaAlta.dia<1 || c.fechaAlta.dia>31 || c.fechaAlta.mes<1 || c.fechaAlta.mes>12 || c.fechaAlta.anio<2000){
            printf("Error: Fecha invalida. Intente nuevamente.\n");
        }
    }while(c.fechaAlta.dia<1 || c.fechaAlta.dia>31 || c.fechaAlta.mes<1 || c.fechaAlta.mes>12 || c.fechaAlta.anio<2000);

    c.estado=CLIENTE_ACTIVO;

    if(guardarRegistro(ARCHIVO_CLIENTES, &c, sizeof(Cliente)) < 0) {
        printf("Error: No se pudo guardar el cliente.\n");
    } else {
        printf("Cliente agregado correctamente.\n");
    }
}

void listarClientes(void) {
    Cliente c;
    int i;
    int cantidad = leerRegistros(
        ARCHIVO_CLIENTES, //Ruta
        &c, //Registros
        sizeof(Cliente), //TamRegistro
        MAX_CLIENTES //Capacidad
    );
    printf("\n--- Lista de clientes ---\n");

    if(cantidad < 0) {
        printf("Error: No hay clientes registrados.\n");
        return;
    }

    //Mostrar datos si el Cliente esta activo
    for(i=0;i<cantidad;i++) {
        if(c.estado == CLIENTE_ACTIVO) {
            printf("DNI: %s\n", c.dni);
            printf("Nombre y Apellido: %s\n", c.nombre);
            printf("Telefono: %s\n", c.telefono);
            printf("Email: %s\n", c.email);
            printf("Direccion: %s\n", c.direccion);
            printf("Fecha de alta: %02d/%02d/%04d\n", c.fechaAlta.dia, c.fechaAlta.mes, c.fechaAlta.anio);
            printf("-------------------------\n");
            //Tambien se puede leer con mostrarCliente(&c[i]);
        }
    }

}

void consultarCliente(void) {
    Cliente c[MAX_CLIENTES];
    char dni[12];
    int posicion;

    printf("\n--- Consultar cliente ---\n");
    //Pedir DNI al usuario
    pedirDNI(dni);
    //Comprobar si el cliente existe en el archivo
    posicion = buscarClientePorDNI(dni);
    if(posicion < 0) {
        printf("Error: No se encontro un cliente con ese DNI.\n");
        return;
    }


    // Mostrar los datos del cliente encontrado
    leerRegistro(ARCHIVO_CLIENTES, &c[posicion], sizeof(Cliente), posicion);
    mostrarCliente(&c[posicion]);

}

void modificarCliente(void) {
    Cliente c[MAX_CLIENTES];
    char dni[12];
    int posicion,cantidad;

    printf("\n--- Modificar cliente ---\n");
    
    //Pedir DNI al usuario
    pedirDNI(dni);

    //Comprobar si el cliente existe en el archivo
    posicion = buscarClientePorDNI(dni);
    if(posicion < 0) {
        printf("Error: No se encontro un cliente con ese DNI.\n");
        return;
    }

    cantidad = leerRegistros(
        ARCHIVO_CLIENTES, //Ruta
        c, //Registros
        sizeof(Cliente), //TamRegistro
        MAX_CLIENTES //Capacidad
    );

    //Pedir nuevos datos del cliente al usuario
    leerlinea("Nuevo Nombre y Apellido: ", c[posicion].nombre, sizeof(c[posicion].nombre));
    leerLinea("Nuevo Telefono: ", c[posicion].telefono, sizeof(c[posicion].telefono));
    leerLinea("Nuevo Email: ", c[posicion].email, sizeof(c[posicion].email));
    leerLinea("Nueva Direccion: ", c[posicion].direccion, sizeof(c[posicion].direccion));

    if(guardarRegistro(ARCHIVO_CLIENTES, &c[posicion], sizeof(Cliente)) < 0) {
        printf("Error: No se pudo modificar el cliente.\n");
    } else {
        printf("Cliente modificado correctamente.\n");
    }
}

void bajaLogicaCliente(void) {
    Cliente c[MAX_CLIENTES];
    char dni[12];
    int posicion,cantidad;

    printf("\n--- Baja de cliente ---\n");
    //Pedir DNI al usuario
    pedirDNI(dni);

    //Comprobar si el cliente existe en el archivo
    posicion = buscarClientePorDNI(dni);
    if(posicion < 0) {
        printf("Error: No se encontro un cliente con ese DNI.\n");
        return;
    }

    cantidad = leerRegistros(
        ARCHIVO_CLIENTES, //Ruta
        c, //Registros
        sizeof(Cliente), //TamRegistro
        MAX_CLIENTES //Capacidad
    );

    //Buscar cliente y mostrar su estado si ya esta inactivo
    if(c[posicion].estado == CLIENTE_INACTIVO) {
        printf("Error: El cliente ya Inactivo.\n");
        return;
    }

    //Sino, cambiar el estado a inactivo y guardar el registro
    c[posicion].estado = CLIENTE_INACTIVO;
    if(guardarRegistro(ARCHIVO_CLIENTES, &c[posicion], sizeof(Cliente)) < 0) {
        printf("Error: No se pudo dar de baja al cliente.\n");
    } else {
        printf("Cliente dado de baja correctamente.\n");
    }
    
}
