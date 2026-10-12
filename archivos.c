
#include <stdio.h>
#include "archivos.h"

int leerRegistros(
    const char *ruta,
    void *registros,
    size_t tamRegistro,
    int capacidad
) {
    FILE *archivo = fopen(ruta, "rb");

    if (archivo == NULL) {
        return -1;
    }

    int cantidad = (int)fread(
        registros,
        tamRegistro,
        capacidad,
        archivo
    );

    if (ferror(archivo)) {
        fclose(archivo);
        return -1;
    }

    fclose(archivo);
    return cantidad;
}

int guardarRegistros(
    const char *ruta,
    const void *registros,
    size_t tamRegistro,
    int cantidad
) {
    FILE *archivo = fopen(ruta, "wb");

    if (archivo == NULL) {
        return 0;
    }

    size_t escritos = fwrite(
        registros,
        tamRegistro,
        cantidad,
        archivo
    );

    fclose(archivo);

    return escritos == (size_t)cantidad;
}