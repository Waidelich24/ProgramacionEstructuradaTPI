#ifndef CLIENTES_H
#define CLIENTES_H

#include "estructuras.h"
#include "errores.h"

/* Devuelve la posicion del cliente en el archivo, -1 si no existe
   o -2 si el archivo no pudo leerse. */
int buscarClientePorDNI(const char dni[]);

/* Carga el cliente en *cliente. Devuelve ERR_OK, ERR_NO_ENCONTRADO
   o un error de archivo. */
CodigoError obtenerCliente(const char dni[], Cliente *cliente);

void menuClientes(void);
void altaCliente(void);
void listarClientes(void);
void consultarCliente(void);
void modificarCliente(void);
void bajaLogicaCliente(void);

#endif
