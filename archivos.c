#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>

#ifdef _WIN32
#include <direct.h>
#define CREAR_CARPETA(ruta) _mkdir(ruta)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define CREAR_CARPETA(ruta) mkdir(ruta, 0755)
#endif

#include "archivos.h"

/* Abre el archivo (lo crea si no existe) para comprobar que se puede usar */
int comprobarArchivo(const char *ruta)
{
    FILE *archivo = fopen(ruta, "ab");
    int ok = 0;

    if (archivo == NULL) {
        printf("Error: no se puede abrir ni crear el archivo %s\n", ruta);
    } else {
        fclose(archivo);
        ok = 1;
    }

    return ok;
}

int inicializarArchivos(void)
{
    int ok;

    CREAR_CARPETA(DIRECTORIO_DATOS);

    ok = comprobarArchivo(ARCHIVO_CLIENTES);
    ok = comprobarArchivo(ARCHIVO_VEHICULOS) && ok;
    ok = comprobarArchivo(ARCHIVO_ALQUILERES) && ok;
    ok = comprobarArchivo(ARCHIVO_MOVIMIENTOS) && ok;

    return ok;
}

long contarRegistros(const char *ruta, size_t tamano)
{
    FILE *archivo = fopen(ruta, "rb");
    long cantidad = -1;
    long bytes;

    if (archivo != NULL) {
        fseek(archivo, 0, SEEK_END);
        bytes = ftell(archivo);
        cantidad = bytes / (long)tamano;
        fclose(archivo);
    }

    return cantidad;
}

int leerRegistroEn(const char *ruta, void *registro, size_t tamano, long pos)
{
    FILE *archivo = fopen(ruta, "rb");
    int ok = 0;

    if (archivo != NULL) {
        fseek(archivo, pos * (long)tamano, SEEK_SET);

        if (fread(registro, tamano, 1, archivo) == 1) {
            ok = 1;
        }

        fclose(archivo);
    }

    return ok;
}

int escribirRegistroEn(const char *ruta, const void *registro, size_t tamano, long pos)
{
    FILE *archivo = fopen(ruta, "r+b");
    int ok = 0;

    if (archivo != NULL) {
        fseek(archivo, pos * (long)tamano, SEEK_SET);

        if (fwrite(registro, tamano, 1, archivo) == 1) {
            ok = 1;
        }

        fclose(archivo);
    }

    return ok;
}

int agregarRegistro(const char *ruta, const void *registro, size_t tamano)
{
    FILE *archivo = fopen(ruta, "ab");
    int ok = 0;

    if (archivo != NULL) {
        if (fwrite(registro, tamano, 1, archivo) == 1) {
            ok = 1;
        }

        fclose(archivo);
    }

    return ok;
}

int vaciarArchivo(const char *ruta)
{
    FILE *archivo = fopen(ruta, "wb");
    int ok = 0;

    if (archivo != NULL) {
        fclose(archivo);
        ok = 1;
    }

    return ok;
}

FILE *abrirLectura(const char *ruta)
{
    return fopen(ruta, "rb");
}

int leerSiguiente(FILE *archivo, void *registro, size_t tamano)
{
    return fread(registro, tamano, 1, archivo) == 1;
}

void cerrarArchivo(FILE *archivo)
{
    fclose(archivo);
}
