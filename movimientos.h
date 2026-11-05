#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "estructuras.h"

void menuMovimientos(void);
void registrarPago(void);
void consultarMovimientos(void);

/* Agrega el movimiento a movimientos.dat (le pone el numero) y una linea
   al archivo de texto del cliente. Devuelve 1 si salio bien */
int registrarMovimiento(Movimiento movimiento);

/* Vuelve a crear movimientos_DNI.txt con los movimientos del cliente (uno por linea). Devuelve 1 si salio bien */
int generarHistorialCliente(const char dni[]);

/* Muestra los movimientos de un cliente. Devuelve cuantos son (-1 si hay error) */
int mostrarHistorialCliente(const char dni[]);

void mostrarTablaMovimientos(void);
void mostrarFilaMovimiento(const Movimiento *movimiento);

#endif
