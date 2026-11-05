#ifndef CLIENTES_H
#define CLIENTES_H

#include "estructuras.h"
#include "errores.h"

<<<<<<< HEAD
/* Devuelve la posicion del cliente en el archivo, -1 si no existe y -2 si hay error */
int buscarClientePorDNI(const char dni[]);

/* Carga el cliente. Devuelve 1 si existe, 0 si no existe y -1 si no se pudo abrir el archivo */
int obtenerCliente(const char dni[], Cliente *cliente);
=======
/* Devuelve la posicion del cliente en el archivo, -1 si no existe
   o -2 si el archivo no pudo leerse. */
int buscarClientePorDNI(const char dni[]);

/* Carga el cliente en *cliente. Devuelve ERR_OK, ERR_NO_ENCONTRADO
   o un error de archivo. */
CodigoError obtenerCliente(const char dni[], Cliente *cliente);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

void menuClientes(void);
void altaCliente(void);
void listarClientes(void);
void consultarCliente(void);
void modificarCliente(void);
void bajaLogicaCliente(void);

#endif
