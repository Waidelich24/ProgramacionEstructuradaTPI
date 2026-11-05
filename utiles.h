#ifndef UTILES_H
#define UTILES_H

#include "estructuras.h"

/* Las funciones pedirXxx devuelven 1 si el usuario ingreso un dato valido
   y 0 si cancelo (escribiendo 0). Si el dato es incorrecto lo vuelven a pedir. */

void mostrarTitulo(const char *titulo);
void mostrarCancelado(void);

int leerLinea(const char *mensaje, char texto[], int tam);
int leerOpcion(const char *mensaje);
int confirmar(const char *mensaje);
int pedirTexto(const char *mensaje, char texto[], int tam);
int pedirEntero(const char *mensaje, int min, int max, int *valor);
int pedirMonto(const char *mensaje, float maximo, float *valor);
int pedirDNI(char dni[]);
int pedirPatente(char patente[]);
int pedirFecha(const char *titulo, Fecha *fecha);   /* solo se usa en las consultas */

int patenteValida(const char *patente);

/* Fechas. Todas las operaciones usan fechaActual() (la fecha de la maquina) */
Fecha fechaActual(void);
int fechaValida(Fecha fecha);
long fechaADias(Fecha fecha);
int compararFechas(Fecha a, Fecha b);
int diasEntre(Fecha desde, Fecha hasta);
Fecha sumarDias(Fecha fecha, int dias);
void imprimirFecha(Fecha fecha);

const char *textoEstadoCliente(EstadoCliente estado);
const char *textoEstadoVehiculo(EstadoVehiculo estado);
const char *textoEstadoAlquiler(EstadoAlquiler estado);
const char *textoTipoMovimiento(TipoMovimiento tipo);

#endif
