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
<<<<<<< HEAD
    int ok = 0;

    if (archivo == NULL) {
        printf("Error: no se puede abrir ni crear el archivo %s\n", ruta);
    } else {
        fclose(archivo);
        ok = 1;
    }

    return ok;
=======
    int existe = 0;

    if (archivo == NULL) {
        printf("Error: no se puede abrir ni crear el archivo '%s'.\n", ruta);
    } else {
        fclose(archivo);
        existe = 1;
    }

    return existe;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

int inicializarArchivos(void)
{
    int ok;
<<<<<<< HEAD

    CREAR_CARPETA(DIRECTORIO_DATOS);

    ok = comprobarArchivo(ARCHIVO_CLIENTES);
    ok = comprobarArchivo(ARCHIVO_VEHICULOS) && ok;
    ok = comprobarArchivo(ARCHIVO_ALQUILERES) && ok;
    ok = comprobarArchivo(ARCHIVO_MOVIMIENTOS) && ok;
=======

    /* Si el directorio ya existe, mkdir falla y no es un problema:
       la verificacion real es abrir cada archivo. */
    CREAR_DIRECTORIO(DIRECTORIO_DATOS);

    ok = asegurarArchivo(ARCHIVO_CLIENTES);
    ok = asegurarArchivo(ARCHIVO_VEHICULOS) && ok;
    ok = asegurarArchivo(ARCHIVO_ALQUILERES) && ok;
    ok = asegurarArchivo(ARCHIVO_MOVIMIENTOS) && ok;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd

    return ok;
}

<<<<<<< HEAD
long contarRegistros(const char *ruta, size_t tamano)
=======
long contarRegistros(const char *ruta, size_t tamRegistro)
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
{
    FILE *archivo = fopen(ruta, "rb");
    long cantidad = -1;
    long bytes;

    if (archivo != NULL) {
<<<<<<< HEAD
        fseek(archivo, 0, SEEK_END);
        bytes = ftell(archivo);
        cantidad = bytes / (long)tamano;
=======
        if (fseek(archivo, 0L, SEEK_END) == 0) {
            bytes = ftell(archivo);

            /* Un tamanio que no es multiplo del registro indica dano */
            if (bytes >= 0 && tamRegistro > 0 &&
                (size_t)bytes % tamRegistro == 0) {
                cantidad = (long)((size_t)bytes / tamRegistro);
            }
        }

>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        fclose(archivo);
    }

    return cantidad;
}

<<<<<<< HEAD
int leerRegistroEn(const char *ruta, void *registro, size_t tamano, long pos)
{
    FILE *archivo = fopen(ruta, "rb");
    int ok = 0;

    if (archivo != NULL) {
        fseek(archivo, pos * (long)tamano, SEEK_SET);

        if (fread(registro, tamano, 1, archivo) == 1) {
            ok = 1;
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        }

        fclose(archivo);
    }

<<<<<<< HEAD
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
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

FILE *abrirLectura(const char *ruta)
{
    return fopen(ruta, "rb");
}

<<<<<<< HEAD
int leerSiguiente(FILE *archivo, void *registro, size_t tamano)
{
    return fread(registro, tamano, 1, archivo) == 1;
=======
int leerSiguiente(FILE *archivo, void *registro, size_t tamRegistro)
{
    return fread(registro, tamRegistro, 1, archivo) == 1;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

void cerrarArchivo(FILE *archivo)
{
<<<<<<< HEAD
    fclose(archivo);
=======
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
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}
