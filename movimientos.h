
#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "estructuras.h"

void registrarPago(void);
void registrarMovimiento(Movimiento movimiento);
void consultarMovimientos(void);
void generarHistorialCliente(const char dni[]);

#endif