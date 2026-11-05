#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stdio.h>
<<<<<<< HEAD
=======
#include <stddef.h>
#include "errores.h"
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

#define DIRECTORIO_DATOS     "datos"
#define ARCHIVO_CLIENTES     "datos/clientes.dat"
#define ARCHIVO_VEHICULOS    "datos/vehiculos.dat"
#define ARCHIVO_ALQUILERES   "datos/alquileres.dat"
#define ARCHIVO_MOVIMIENTOS  "datos/movimientos.dat"

<<<<<<< HEAD
/* Los archivos no se cargan en memoria: cada operacion lee o escribe
   directamente en el archivo, de a un registro por vez. */

/* Crea la carpeta y los archivos si no existen. Devuelve 1 si todo esta bien */
int inicializarArchivos(void);

/* Cantidad de registros del archivo (-1 si no se pudo abrir) */
long contarRegistros(const char *ruta, size_t tamano);

/* Leen, escriben o agregan un registro. Devuelven 1 si salio bien y 0 si no.
   La posicion 0 es el primer registro del archivo. */
int leerRegistroEn(const char *ruta, void *registro, size_t tamano, long pos);
int escribirRegistroEn(const char *ruta, const void *registro, size_t tamano, long pos);
int agregarRegistro(const char *ruta, const void *registro, size_t tamano);

/* Deja el archivo vacio (lo usa el programa que genera los datos de prueba) */
int vaciarArchivo(const char *ruta);

/* Lectura de todos los registros: abrirLectura, leerSiguiente, cerrarArchivo */
FILE *abrirLectura(const char *ruta);
int leerSiguiente(FILE *archivo, void *registro, size_t tamano);
void cerrarArchivo(FILE *archivo);
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

#endif
