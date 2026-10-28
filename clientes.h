
#ifndef CLIENTES_H
#define CLIENTES_H

#include "estructuras.h"

int buscarClientePorDNI(const char dni[]);
void altaCliente(void);
void listarClientes(void);
void consultarCliente(void);
void modificarCliente(void);
void bajaLogicaCliente(void);
void menuClientes(void);

#endif
