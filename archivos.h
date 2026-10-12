
#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stddef.h>

int leerRegistros(
    const char *ruta,
    void *registros,
    size_t tamRegistro,
    int capacidad
);

int guardarRegistros(
    const char *ruta,
    const void *registros,
    size_t tamRegistro,
    int cantidad
);

#endif