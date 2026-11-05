#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "estructuras.h"
#include "errores.h"

void menuMovimientos(void);
void registrarPago(void);
void consultarMovimientos(void);

<<<<<<< HEAD
/* Agrega el movimiento a movimientos.dat (le pone el numero) y una linea
   al archivo de texto del cliente. Devuelve 1 si salio bien */
int registrarMovimiento(Movimiento movimiento);

/* Vuelve a crear movimientos_DNI.txt con los movimientos del cliente (uno por linea). Devuelve 1 si salio bien */
int generarHistorialCliente(const char dni[]);
=======
/* Agrega el movimiento a movimientos.dat (asigna el numero) y una linea al
   archivo de texto del cliente. Devuelve ERR_OK o el error producido. */
CodigoError registrarMovimiento(Movimiento movimiento);

/* Regenera movimientos_DNI.txt a partir de movimientos.dat (un registro por
   linea). Devuelve ERR_OK o el error producido. */
CodigoError generarHistorialCliente(const char dni[]);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

/* Muestra los movimientos de un cliente. Devuelve cuantos son (-1 si hay error) */
int mostrarHistorialCliente(const char dni[]);

void mostrarTablaMovimientos(void);
void mostrarFilaMovimiento(const Movimiento *movimiento);

#endif
