#ifndef ALQUILERES_H
#define ALQUILERES_H

#include "estructuras.h"

void menuAlquileres(void);
void realizarAlquiler(void);
void registrarDevolucion(void);
void modificarAlquiler(void);
void cancelarAlquiler(void);
void listarAlquileres(void);

/* Devuelve la posicion del alquiler en el archivo o -1 si no existe / error */
int buscarAlquilerPorNumero(int numero);

/* Devuelve 1 si el cliente tiene algun alquiler activo, 0 si no y -1 si error */
int clienteTieneAlquilerActivo(const char dni[]);

/* Saldo pendiente de un alquiler (0 si esta cancelado o totalmente pagado) */
float saldoAlquiler(const Alquiler *alquiler);

/* Tabla comun para mostrar alquileres (listados y consultas) */
void imprimirEncabezadoAlquileres(void);
void imprimirFilaAlquiler(const Alquiler *alquiler);

#endif
