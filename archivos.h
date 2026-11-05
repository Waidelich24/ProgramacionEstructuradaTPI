#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stdio.h>

#define DIRECTORIO_DATOS     "datos"
#define ARCHIVO_CLIENTES     "datos/clientes.dat"
#define ARCHIVO_VEHICULOS    "datos/vehiculos.dat"
#define ARCHIVO_ALQUILERES   "datos/alquileres.dat"
#define ARCHIVO_MOVIMIENTOS  "datos/movimientos.dat"

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

#endif
