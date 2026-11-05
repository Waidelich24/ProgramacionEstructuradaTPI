#ifndef VEHICULOS_H
#define VEHICULOS_H

#include "estructuras.h"

/* Devuelve la posicion del vehiculo en el archivo, -1 si no existe y -2 si hay error */
int buscarVehiculoPorPatente(const char patente[]);

/* Carga el vehiculo. Devuelve 1 si existe, 0 si no existe y -1 si no se pudo abrir el archivo */
int obtenerVehiculo(const char patente[], Vehiculo *vehiculo);

/* Cambia el estado del vehiculo directamente en el archivo. Devuelve 1 si salio bien */
int actualizarEstadoVehiculo(const char patente[], EstadoVehiculo estado);

void menuVehiculos(void);
void altaVehiculo(void);
void listarVehiculos(void);
void consultarVehiculo(void);
void modificarVehiculo(void);

/* Muestra los disponibles y devuelve cuantos son (-1 si hay error) */
int listarVehiculosDisponibles(void);

#endif
