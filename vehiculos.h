#ifndef VEHICULOS_H
#define VEHICULOS_H

#include "estructuras.h"
#include "errores.h"

<<<<<<< HEAD
/* Devuelve la posicion del vehiculo en el archivo, -1 si no existe y -2 si hay error */
int buscarVehiculoPorPatente(const char patente[]);

/* Carga el vehiculo. Devuelve 1 si existe, 0 si no existe y -1 si no se pudo abrir el archivo */
int obtenerVehiculo(const char patente[], Vehiculo *vehiculo);

/* Cambia el estado del vehiculo directamente en el archivo. Devuelve 1 si salio bien */
int actualizarEstadoVehiculo(const char patente[], EstadoVehiculo estado);
=======
/* Devuelve la posicion del vehiculo en el archivo, -1 si no existe
   o -2 si el archivo no pudo leerse. */
int buscarVehiculoPorPatente(const char patente[]);

/* Carga el vehiculo en *vehiculo. Devuelve ERR_OK, ERR_NO_ENCONTRADO
   o un error de archivo. */
CodigoError obtenerVehiculo(const char patente[], Vehiculo *vehiculo);

/* Cambia el estado de un vehiculo directamente en el archivo */
CodigoError actualizarEstadoVehiculo(const char patente[],
                                     EstadoVehiculo estado);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

void menuVehiculos(void);
void altaVehiculo(void);
void listarVehiculos(void);
void consultarVehiculo(void);
void modificarVehiculo(void);

/* Muestra los disponibles y devuelve cuantos son (-1 si hay error) */
int listarVehiculosDisponibles(void);

#endif
