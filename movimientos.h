#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "estructuras.h"

void menuMovimientos(void);
void registrarPago(void);
void consultarMovimientos(void);
int registrarMovimiento(Movimiento movimiento);
void generarHistorialCliente(const char dni[]);

#endif
