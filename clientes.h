#ifndef CLIENTES_H
#define CLIENTES_H

#include "estructuras.h"

/* Devuelve la posicion del cliente en el archivo, -1 si no existe y -2 si hay error */
int buscarClientePorDNI(const char dni[]);

/* Carga el cliente. Devuelve 1 si existe, 0 si no existe y -1 si no se pudo abrir el archivo */
int obtenerCliente(const char dni[], Cliente *cliente);

void menuClientes(void);
void altaCliente(void);
void listarClientes(void);
void consultarCliente(void);
void modificarCliente(void);
void bajaLogicaCliente(void);

#endif
