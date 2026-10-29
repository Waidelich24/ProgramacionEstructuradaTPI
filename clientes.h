#ifndef CLIENTES_H
#define CLIENTES_H

#include "estructuras.h"

/* Devuelve la posicion del cliente en el archivo o -1 si no existe / error */
int buscarClientePorDNI(const char dni[]);

/* Carga el cliente en *cliente. Devuelve 1 si existe, 0 si no existe
   y -1 si el archivo no pudo leerse. */
int obtenerCliente(const char dni[], Cliente *cliente);

void menuClientes(void);
void altaCliente(void);
void listarClientes(void);
void consultarCliente(void);
void modificarCliente(void);
void bajaLogicaCliente(void);

#endif
