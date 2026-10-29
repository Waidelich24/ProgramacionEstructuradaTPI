#ifndef VEHICULOS_H
#define VEHICULOS_H

#include "estructuras.h"

/* Devuelve la posicion del vehiculo en el archivo o -1 si no existe / error */
int buscarVehiculoPorPatente(const char patente[]);

/* Carga el vehiculo en *vehiculo. Devuelve 1 si existe, 0 si no existe
   y -1 si el archivo no pudo leerse. */
int obtenerVehiculo(const char patente[], Vehiculo *vehiculo);

/* Cambia el estado de un vehiculo y lo guarda. Devuelve 1 si tuvo exito. */
int actualizarEstadoVehiculo(const char patente[], EstadoVehiculo estado);

void menuVehiculos(void);
void altaVehiculo(void);
void listarVehiculos(void);
void consultarVehiculo(void);
void modificarVehiculo(void);

/* Lista los disponibles. Devuelve la cantidad mostrada o -1 si hubo error. */
int listarVehiculosDisponibles(void);

#endif
