#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stdio.h>
#include <stddef.h>
#include "errores.h"

/* Rutas de los archivos del sistema */
#define DIRECTORIO_DATOS     "datos"
#define ARCHIVO_CLIENTES     "datos/clientes.dat"
#define ARCHIVO_VEHICULOS    "datos/vehiculos.dat"
#define ARCHIVO_ALQUILERES   "datos/alquileres.dat"
#define ARCHIVO_MOVIMIENTOS  "datos/movimientos.dat"

/*
 * Capa de acceso a archivos. No hay limite de registros: cada operacion
 * lee o escribe directamente sobre el archivo, de a un registro por vez.
 */

/* Crea la carpeta y los archivos binarios si no existen y verifica que
   puedan abrirse. Devuelve 1 si todo esta correcto. */
int inicializarArchivos(void);

/* Cantidad de registros del archivo, o -1 si no puede abrirse o esta danado */
long contarRegistros(const char *ruta, size_t tamRegistro);

/* Acceso directo por posicion (0 = primer registro) */
CodigoError leerRegistroEn(const char *ruta, void *registro,
                           size_t tamRegistro, long posicion);
CodigoError escribirRegistroEn(const char *ruta, const void *registro,
                               size_t tamRegistro, long posicion);

/* Agrega un registro al final del archivo */
CodigoError agregarRegistro(const char *ruta, const void *registro,
                            size_t tamRegistro);

/* Deja el archivo vacio (se usa solo para regenerar datos de prueba) */
CodigoError vaciarArchivo(const char *ruta);

/* Lectura secuencial: abrirLectura / leerSiguiente / cerrarArchivo */
FILE *abrirLectura(const char *ruta);
int   leerSiguiente(FILE *archivo, void *registro, size_t tamRegistro);
void  cerrarArchivo(FILE *archivo);

/* Traduce el resultado de una busqueda (posicion >= 0, -1 no existe,
   -2 error de archivo) a un codigo de error */
CodigoError errorPorPosicion(long posicion);

#endif
