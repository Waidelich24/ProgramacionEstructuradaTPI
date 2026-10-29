#ifndef UTILES_H
#define UTILES_H

#include "estructuras.h"

#define ANIO_MINIMO 1900
#define ANIO_MAXIMO 2100

/*
 * Convencion de las funciones pedirXxx: devuelven 1 si el usuario ingreso un
 * dato valido y 0 si cancelo (ingresando 0 o por fin de entrada). Mientras el
 * dato sea invalido se informa el problema y se vuelve a pedir.
 */

/* ---------- Presentacion ---------- */
void mostrarTitulo(const char *titulo);
void mostrarCancelado(void);

/* ---------- Entrada por teclado (siempre por lineas, sin scanf) ---------- */
int  leerLinea(const char *mensaje, char texto[], int tam);
int  leerOpcion(const char *mensaje);   /* -1 si es invalida, 0 si fin de entrada */
int  confirmar(const char *mensaje);    /* 1 = si, 0 = no */

int  pedirTexto(const char *mensaje, char texto[], int tam);
int  pedirCampo(const char *mensaje, char destino[], int tam,
                int (*validar)(const char *), const char *mensajeError);
int  pedirEntero(const char *mensaje, int min, int max, int *valor);
int  pedirMonto(const char *mensaje, float maximo, float *valor);
int  pedirDNI(char dni[]);
int  pedirPatente(char patente[]);
int  pedirFecha(const char *titulo, Fecha *fecha);
int  pedirFechaOperacion(const char *titulo, Fecha *fecha);

/* ---------- Validaciones ---------- */
int  patenteValida(const char *patente);

/* ---------- Fechas ---------- */
Fecha fechaActual(void);
int   fechaValida(Fecha fecha);
long  fechaADias(Fecha fecha);
Fecha diasAFecha(long dias);
int   compararFechas(Fecha a, Fecha b);   /* -1, 0 o 1 */
int   diasEntre(Fecha desde, Fecha hasta);
Fecha sumarDias(Fecha fecha, int dias);
void  imprimirFecha(Fecha fecha);

/* ---------- Textos de estados ---------- */
const char *textoEstadoCliente(EstadoCliente estado);
const char *textoEstadoVehiculo(EstadoVehiculo estado);
const char *textoEstadoAlquiler(EstadoAlquiler estado);
const char *textoTipoMovimiento(TipoMovimiento tipo);

#endif
