#ifndef ALQUILERES_H
#define ALQUILERES_H

#include "estructuras.h"
#include "errores.h"

void menuAlquileres(void);
void realizarAlquiler(void);
void registrarDevolucion(void);
void modificarAlquiler(void);
void cancelarAlquiler(void);
void listarAlquileres(void);

/* Busca un alquiler por numero y lo copia en *alquiler (puede ser NULL).
   Devuelve la posicion, -1 si no existe o -2 si el archivo no pudo leerse. */
long buscarAlquiler(int numero, Alquiler *alquiler);
int  buscarAlquilerPorNumero(int numero);

/* Devuelve 1 si el cliente tiene algun alquiler activo, 0 si no y -1 si error */
int clienteTieneAlquilerActivo(const char dni[]);

/* Saldo pendiente de un alquiler (0 si esta cancelado o totalmente pagado) */
float saldoAlquiler(const Alquiler *alquiler);

/* Tabla comun para mostrar alquileres (listados y consultas) */
void imprimirEncabezadoAlquileres(void);
void imprimirFilaAlquiler(const Alquiler *alquiler);

#endif
