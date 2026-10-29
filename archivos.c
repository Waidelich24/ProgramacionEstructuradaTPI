#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>

#ifdef _WIN32
#include <direct.h>
#define CREAR_DIRECTORIO(ruta) _mkdir(ruta)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define CREAR_DIRECTORIO(ruta) mkdir(ruta, 0755)
#endif

#include "archivos.h"

/* Crea el archivo vacio si no existe y comprueba que se pueda abrir */
static int asegurarArchivo(const char *ruta)
{
    FILE *archivo = fopen(ruta, "ab");

    if (archivo == NULL) {
        printf("Error: no se puede abrir ni crear el archivo '%s'.\n", ruta);
        return 0;
    }

    fclose(archivo);
    return 1;
}

int inicializarArchivos(void)
{
    /* Si el directorio ya existe, mkdir falla y no es un problema:
       la verificacion real es abrir cada archivo. */
    CREAR_DIRECTORIO(DIRECTORIO_DATOS);

    return asegurarArchivo(ARCHIVO_CLIENTES) &&
           asegurarArchivo(ARCHIVO_VEHICULOS) &&
           asegurarArchivo(ARCHIVO_ALQUILERES) &&
           asegurarArchivo(ARCHIVO_MOVIMIENTOS);
}

int leerRegistros(const char *ruta, void *registros,
                  size_t tamRegistro, int capacidad)
{
    FILE *archivo;
    long bytes;
    long total;
    int cantidad;

    if (capacidad <= 0 || tamRegistro == 0) {
        return -1;
    }

    archivo = fopen(ruta, "rb");

    if (archivo == NULL) {
        return -1;
    }

    if (fseek(archivo, 0L, SEEK_END) != 0) {
        fclose(archivo);
        return -1;
    }

    bytes = ftell(archivo);

    if (bytes < 0 || fseek(archivo, 0L, SEEK_SET) != 0) {
        fclose(archivo);
        return -1;
    }

    /* Archivo danado o con mas registros de los que caben en memoria */
    if ((size_t)bytes % tamRegistro != 0) {
        fclose(archivo);
        return -1;
    }

    total = (long)((size_t)bytes / tamRegistro);

    if (total > capacidad) {
        fclose(archivo);
        return -1;
    }

    cantidad = (int)fread(registros, tamRegistro, (size_t)total, archivo);

    if (ferror(archivo) || cantidad != (int)total) {
        fclose(archivo);
        return -1;
    }

    fclose(archivo);
    return cantidad;
}

int guardarRegistros(const char *ruta, const void *registros,
                     size_t tamRegistro, int cantidad)
{
    FILE *archivo;
    size_t escritos;

    if (cantidad < 0) {
        return 0;
    }

    archivo = fopen(ruta, "wb");

    if (archivo == NULL) {
        return 0;
    }

    escritos = 0;

    if (cantidad > 0) {
        escritos = fwrite(registros, tamRegistro, (size_t)cantidad, archivo);
    }

    if (fclose(archivo) != 0) {
        return 0;
    }

    return escritos == (size_t)cantidad;
}

int agregarRegistro(const char *ruta, const void *registro,
                    size_t tamRegistro)
{
    FILE *archivo = fopen(ruta, "ab");
    size_t escritos;

    if (archivo == NULL) {
        return 0;
    }

    escritos = fwrite(registro, tamRegistro, 1, archivo);

    if (fclose(archivo) != 0) {
        return 0;
    }

    return escritos == 1;
}
