#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "estructuras.h"

void menuMovimientos(void);
void registrarPago(void);
void consultarMovimientos(void);

/* Agrega el movimiento al archivo binario (asigna el numero) y actualiza
   el archivo de texto del cliente. Devuelve 1 si se registro. */
int registrarMovimiento(Movimiento movimiento);

/* Regenera movimientos_DNI.txt a partir de movimientos.dat (un registro por
   linea). Devuelve 1 si se genero correctamente. */
int generarHistorialCliente(const char dni[]);

/* Muestra por pantalla los movimientos de un cliente.
   Devuelve la cantidad mostrada o -1 si hubo error. */
int mostrarHistorialCliente(const char dni[]);

/* Tabla comun para mostrar movimientos (listados y consultas) */
void imprimirEncabezadoMovimientos(void);
void imprimirFilaMovimiento(const Movimiento *movimiento);

#endif
