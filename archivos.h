#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stddef.h>

/* Rutas de los archivos del sistema */
#define DIRECTORIO_DATOS     "datos"
#define ARCHIVO_CLIENTES     "datos/clientes.dat"
#define ARCHIVO_VEHICULOS    "datos/vehiculos.dat"
#define ARCHIVO_ALQUILERES   "datos/alquileres.dat"
#define ARCHIVO_MOVIMIENTOS  "datos/movimientos.dat"

/* Capacidad maxima de registros (unica definicion para todos los modulos) */
#define MAX_CLIENTES     500
#define MAX_VEHICULOS    500
#define MAX_ALQUILERES   1000
#define MAX_MOVIMIENTOS  3000

/*
 * Crea el directorio de datos y los archivos binarios si no existen,
 * verificando que puedan abrirse. Devuelve 1 si todo esta correcto.
 */
int inicializarArchivos(void);

/*
 * Lee hasta 'capacidad' registros. Devuelve la cantidad leida o -1 si el
 * archivo no puede abrirse, esta danado (tamanio no multiplo del registro)
 * o contiene mas registros que la capacidad (evita perder datos al guardar).
 */
int leerRegistros(const char *ruta, void *registros,
                  size_t tamRegistro, int capacidad);

/* Reescribe el archivo completo. Devuelve 1 si se guardo correctamente. */
int guardarRegistros(const char *ruta, const void *registros,
                     size_t tamRegistro, int cantidad);

/* Agrega un registro al final del archivo. Devuelve 1 si tuvo exito. */
int agregarRegistro(const char *ruta, const void *registro,
                    size_t tamRegistro);

#endif
