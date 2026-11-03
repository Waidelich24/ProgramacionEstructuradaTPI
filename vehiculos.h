#ifndef VEHICULOS_H
#define VEHICULOS_H

#include "estructuras.h"
#include "errores.h"

/* Devuelve la posicion del vehiculo en el archivo, -1 si no existe
   o -2 si el archivo no pudo leerse. */
int buscarVehiculoPorPatente(const char patente[]);

/* Carga el vehiculo en *vehiculo. Devuelve ERR_OK, ERR_NO_ENCONTRADO
   o un error de archivo. */
CodigoError obtenerVehiculo(const char patente[], Vehiculo *vehiculo);

/* Cambia el estado de un vehiculo directamente en el archivo */
CodigoError actualizarEstadoVehiculo(const char patente[],
                                     EstadoVehiculo estado);

void menuVehiculos(void);
void altaVehiculo(void);
void listarVehiculos(void);
void consultarVehiculo(void);
void modificarVehiculo(void);

/* Lista los disponibles. Devuelve la cantidad mostrada o -1 si hubo error. */
int listarVehiculosDisponibles(void);

#endif
