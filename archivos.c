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
    int existe = 0;

    if (archivo == NULL) {
        printf("Error: no se puede abrir ni crear el archivo '%s'.\n", ruta);
    } else {
        fclose(archivo);
        existe = 1;
    }

    return existe;
}

int inicializarArchivos(void)
{
    int ok;

    /* Si el directorio ya existe, mkdir falla y no es un problema:
       la verificacion real es abrir cada archivo. */
    CREAR_DIRECTORIO(DIRECTORIO_DATOS);

    ok = asegurarArchivo(ARCHIVO_CLIENTES);
    ok = asegurarArchivo(ARCHIVO_VEHICULOS) && ok;
    ok = asegurarArchivo(ARCHIVO_ALQUILERES) && ok;
    ok = asegurarArchivo(ARCHIVO_MOVIMIENTOS) && ok;

    return ok;
}

long contarRegistros(const char *ruta, size_t tamRegistro)
{
    FILE *archivo = fopen(ruta, "rb");
    long cantidad = -1;
    long bytes;

    if (archivo != NULL) {
        if (fseek(archivo, 0L, SEEK_END) == 0) {
            bytes = ftell(archivo);

            /* Un tamanio que no es multiplo del registro indica dano */
            if (bytes >= 0 && tamRegistro > 0 &&
                (size_t)bytes % tamRegistro == 0) {
                cantidad = (long)((size_t)bytes / tamRegistro);
            }
        }

        fclose(archivo);
    }

    return cantidad;
}

CodigoError leerRegistroEn(const char *ruta, void *registro,
                           size_t tamRegistro, long posicion)
{
    FILE *archivo = fopen(ruta, "rb");
    CodigoError error = ERR_ARCHIVO_ABRIR;

    if (archivo != NULL) {
        error = ERR_ARCHIVO_LEER;

        if (posicion >= 0 &&
            fseek(archivo, posicion * (long)tamRegistro, SEEK_SET) == 0 &&
            fread(registro, tamRegistro, 1, archivo) == 1) {
            error = ERR_OK;
        }

        fclose(archivo);
    }

    return error;
}

CodigoError escribirRegistroEn(const char *ruta, const void *registro,
                               size_t tamRegistro, long posicion)
{
    FILE *archivo = fopen(ruta, "r+b");
    CodigoError error = ERR_ARCHIVO_ABRIR;

    if (archivo != NULL) {
        error = ERR_ARCHIVO_ESCRIBIR;

        if (posicion >= 0 &&
            fseek(archivo, posicion * (long)tamRegistro, SEEK_SET) == 0 &&
            fwrite(registro, tamRegistro, 1, archivo) == 1) {
            error = ERR_OK;
        }

        if (fclose(archivo) != 0) {
            error = ERR_ARCHIVO_ESCRIBIR;
        }
    }

    return error;
}

CodigoError agregarRegistro(const char *ruta, const void *registro,
                            size_t tamRegistro)
{
    FILE *archivo = fopen(ruta, "ab");
    CodigoError error = ERR_ARCHIVO_ABRIR;

    if (archivo != NULL) {
        error = ERR_ARCHIVO_ESCRIBIR;

        if (fwrite(registro, tamRegistro, 1, archivo) == 1) {
            error = ERR_OK;
        }

        if (fclose(archivo) != 0) {
            error = ERR_ARCHIVO_ESCRIBIR;
        }
    }

    return error;
}

CodigoError vaciarArchivo(const char *ruta)
{
    FILE *archivo = fopen(ruta, "wb");
    CodigoError error = ERR_ARCHIVO_ABRIR;

    if (archivo != NULL) {
        fclose(archivo);
        error = ERR_OK;
    }

    return error;
}

FILE *abrirLectura(const char *ruta)
{
    return fopen(ruta, "rb");
}

int leerSiguiente(FILE *archivo, void *registro, size_t tamRegistro)
{
    return fread(registro, tamRegistro, 1, archivo) == 1;
}

void cerrarArchivo(FILE *archivo)
{
    if (archivo != NULL) {
        fclose(archivo);
    }
}

CodigoError errorPorPosicion(long posicion)
{
    CodigoError error = ERR_OK;

    if (posicion == -1) {
        error = ERR_NO_ENCONTRADO;
    } else if (posicion < -1) {
        error = ERR_ARCHIVO_LEER;
    }

    return error;
}
