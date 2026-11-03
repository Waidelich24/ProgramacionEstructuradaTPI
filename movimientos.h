#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "estructuras.h"
#include "errores.h"

void menuMovimientos(void);
void registrarPago(void);
void consultarMovimientos(void);

/* Agrega el movimiento a movimientos.dat (asigna el numero) y una linea al
   archivo de texto del cliente. Devuelve ERR_OK o el error producido. */
CodigoError registrarMovimiento(Movimiento movimiento);

/* Regenera movimientos_DNI.txt a partir de movimientos.dat (un registro por
   linea). Devuelve ERR_OK o el error producido. */
CodigoError generarHistorialCliente(const char dni[]);

/* Muestra por pantalla los movimientos de un cliente.
   Devuelve la cantidad mostrada o -1 si hubo error. */
int mostrarHistorialCliente(const char dni[]);

/* Tabla comun para mostrar movimientos (listados y consultas) */
void imprimirEncabezadoMovimientos(void);
void imprimirFilaMovimiento(const Movimiento *movimiento);

#endif
