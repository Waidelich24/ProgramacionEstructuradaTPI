#ifndef ALQUILERES_H
#define ALQUILERES_H

#include "estructuras.h"

void menuAlquileres(void);
void realizarAlquiler(void);
void registrarDevolucion(void);
void modificarAlquiler(void);
void cancelarAlquiler(void);
void listarAlquileres(void);

/* Busca un alquiler por numero. Devuelve la posicion, -1 si no existe y -2 si hay error */
long buscarAlquiler(int numero, Alquiler *alquiler);
int buscarAlquilerPorNumero(int numero);

/* Devuelve 1 si el cliente tiene un alquiler activo, 0 si no y -1 si hay error */
int clienteTieneAlquilerActivo(const char dni[]);

/* Lo que falta pagar de un alquiler (0 si esta cancelado o pagado) */
float saldoAlquiler(const Alquiler *alquiler);

/* Controla que la fecha de la maquina no sea anterior al inicio del alquiler.
   Devuelve 1 si esta bien y 0 (con un mensaje) si no. */
int fechaMaquinaCorrecta(const Alquiler *alquiler, Fecha hoy);

void mostrarTablaAlquileres(void);
void mostrarFilaAlquiler(const Alquiler *alquiler);

#endif
