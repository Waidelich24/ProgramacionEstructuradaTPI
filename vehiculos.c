#include <stdio.h>
#include <string.h>

#include "vehiculos.h"
#include "archivos.h"
#include "utiles.h"

<<<<<<< HEAD
/* Busca un vehiculo por patente leyendo el archivo.
   Devuelve la posicion, -1 si no existe y -2 si no se pudo abrir el archivo */
static long buscarVehiculo(const char patente[], Vehiculo *encontrado)
{
    FILE *archivo = abrirLectura(ARCHIVO_VEHICULOS);
    Vehiculo vehiculo;
    long pos = -1;
    long i = 0;

    if (archivo == NULL) {
        pos = -2;
    } else {
        while (pos == -1 && leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            if (strcmp(vehiculo.patente, patente) == 0) {
                pos = i;
                *encontrado = vehiculo;
            }
            i++;
        }
        cerrarArchivo(archivo);
    }

    return pos;
=======
#define ANIO_VEHICULO_MINIMO 1950
#define CAPACIDAD_MAXIMA     60

/* =========================================================
   ACCESO A ARCHIVO
   ========================================================= */

/* Busca por patente leyendo el archivo registro por registro.
   Devuelve la posicion, -1 si no existe o -2 si no puede abrirse. */
static long buscarVehiculo(const char patente[], Vehiculo *salida)
{
    FILE *archivo = abrirLectura(ARCHIVO_VEHICULOS);
    Vehiculo actual;
    long posicion = -1;
    long indice = 0;

    if (archivo == NULL) {
        posicion = -2;
    } else {
        while (posicion < 0 &&
               leerSiguiente(archivo, &actual, sizeof(Vehiculo))) {
            if (strcmp(actual.patente, patente) == 0) {
                posicion = indice;

                if (salida != NULL) {
                    *salida = actual;
                }
            }

            indice++;
        }

        cerrarArchivo(archivo);
    }

    return posicion;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

int buscarVehiculoPorPatente(const char patente[])
{
<<<<<<< HEAD
    Vehiculo vehiculo;

    return (int)buscarVehiculo(patente, &vehiculo);
=======
    return (int)buscarVehiculo(patente, NULL);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

CodigoError obtenerVehiculo(const char patente[], Vehiculo *vehiculo)
{
<<<<<<< HEAD
    long pos = buscarVehiculo(patente, vehiculo);
    int resultado = 1;

    if (pos == -1) {
        resultado = 0;
    } else if (pos < -1) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
        resultado = -1;
    }

    return resultado;
=======
    return errorPorPosicion(buscarVehiculo(patente, vehiculo));
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

CodigoError actualizarEstadoVehiculo(const char patente[],
                                     EstadoVehiculo estado)
{
    Vehiculo vehiculo;
<<<<<<< HEAD
    long pos = buscarVehiculo(patente, &vehiculo);
    int ok = 0;

    if (pos == -1) {
        printf("Error: no existe el vehiculo %s\n", patente);
    } else if (pos < -1) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
    } else {
        vehiculo.estado = estado;
        ok = escribirRegistroEn(ARCHIVO_VEHICULOS, &vehiculo, sizeof(Vehiculo), pos);

        if (!ok) {
            printf("Error: no se pudo guardar el vehiculo %s en el archivo.\n", patente);
        }
    }

    return ok;
=======
    long posicion = buscarVehiculo(patente, &vehiculo);
    CodigoError error = errorPorPosicion(posicion);

    if (error == ERR_OK) {
        vehiculo.estado = estado;
        error = escribirRegistroEn(ARCHIVO_VEHICULOS, &vehiculo,
                                   sizeof(Vehiculo), posicion);
    }

    return error;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

/* ---------- Pantalla ---------- */

static void mostrarTabla(void)
{
    printf("%-9s %-14s %-16s %-5s %-12s %-4s %12s  %-16s\n", "Patente",
           "Marca", "Modelo", "Anio", "Tipo", "Cap.", "Precio/dia", "Estado");
}

static void mostrarFila(const Vehiculo *v)
{
    printf("%-9s %-14.14s %-16.16s %-5d %-12.12s %-4d %12.2f  %-16s\n",
           v->patente, v->marca, v->modelo, v->anio, v->tipo, v->capacidad,
           v->precioDia, textoEstadoVehiculo(v->estado));
}

static void mostrarVehiculo(const Vehiculo *v)
{
    printf("\n----------------------------------------\n");
    printf("Patente:        %s\n", v->patente);
    printf("Marca:          %s\n", v->marca);
    printf("Modelo:         %s\n", v->modelo);
    printf("Anio:           %d\n", v->anio);
    printf("Tipo:           %s\n", v->tipo);
    printf("Capacidad:      %d\n", v->capacidad);
    printf("Precio por dia: $%.2f\n", v->precioDia);
    printf("Estado:         %s\n", textoEstadoVehiculo(v->estado));
    printf("----------------------------------------\n");
}

/* ---------- Carga de datos ---------- */

/* Pide un texto que entre en "tam" caracteres */
static int pedirTextoCorto(const char *mensaje, char destino[], int tam)
{
    char texto[200];
    int valido = 0;

    while (!valido && pedirTexto(mensaje, texto, 200)) {
        if ((int)strlen(texto) >= tam) {
            printf("Error: el texto es demasiado largo (maximo %d letras).\n", tam - 1);
        } else {
            strcpy(destino, texto);
            valido = 1;
        }
    }

    return valido;
}

/* Pide el dato numero "campo" del vehiculo (1 marca ... 6 precio) */
static int pedirDato(Vehiculo *v, int campo)
{
    int numero;
    float precio;
    int ok = 0;

<<<<<<< HEAD
    if (campo == 1) {
        ok = pedirTextoCorto("Marca", v->marca, 40);
    } else if (campo == 2) {
        ok = pedirTextoCorto("Modelo", v->modelo, 40);
    } else if (campo == 3) {
        ok = pedirEntero("Anio", 1950, fechaActual().anio + 1, &numero);
        if (ok) {
            v->anio = numero;
        }
    } else if (campo == 4) {
        ok = pedirTextoCorto("Tipo (sedan, pick-up, etc.)", v->tipo, 30);
    } else if (campo == 5) {
        ok = pedirEntero("Capacidad de pasajeros", 1, 60, &numero);
        if (ok) {
            v->capacidad = numero;
        }
    } else if (campo == 6) {
        ok = pedirMonto("Precio por dia", 0, &precio);
        if (ok) {
            v->precioDia = precio;
        }
=======
    switch (campo) {
        case 1:
            ok = pedirCampo("Marca", v->marca, sizeof(v->marca), NULL, NULL);
            break;

        case 2:
            ok = pedirCampo("Modelo", v->modelo, sizeof(v->modelo),
                            NULL, NULL);
            break;

        case 3:
            ok = pedirEntero("Anio de fabricacion", ANIO_VEHICULO_MINIMO,
                             fechaActual().anio + 1, &numero);

            if (ok) {
                v->anio = numero;
            }
            break;

        case 4:
            ok = pedirCampo("Tipo (sedan, pick-up, utilitario...)",
                            v->tipo, sizeof(v->tipo), NULL, NULL);
            break;

        case 5:
            ok = pedirEntero("Capacidad de pasajeros", 1, CAPACIDAD_MAXIMA,
                             &numero);

            if (ok) {
                v->capacidad = numero;
            }
            break;

        case 6:
            ok = pedirMonto("Precio por dia", 0.0f, &precio);

            if (ok) {
                v->precioDia = precio;
            }
            break;

        default:
            break;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }

    return ok;
}

<<<<<<< HEAD
/* Cambia el estado a mano. Devuelve 1 si se cambio */
=======
/* Cambia el estado manual (mantenimiento / baja / disponible).
   Devuelve 1 si se eligio un estado nuevo. */
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
static int pedirNuevoEstado(Vehiculo *v)
{
    int opcion;
    int cambiado = 0;

    if (v->estado == VEHICULO_ALQUILADO) {
<<<<<<< HEAD
        printf("Error: el vehiculo esta alquilado. Primero registre la devolucion.\n");
    } else {
        printf("1. Disponible\n2. En mantenimiento\n3. Baja\n0. Cancelar\n");
        opcion = leerOpcion("Nuevo estado: ");

=======
        printf("Error: el vehiculo esta alquilado. Registre primero la "
               "devolucion para poder cambiar su estado.\n");
    } else {
        printf("\nSeleccione el nuevo estado:\n");
        printf("1. Disponible\n");
        printf("2. En mantenimiento\n");
        printf("3. Baja\n");
        printf("0. Cancelar\n");

        opcion = leerOpcion("Opcion: ");

>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        if (opcion == 1) {
            v->estado = VEHICULO_DISPONIBLE;
            cambiado = 1;
        } else if (opcion == 2) {
            v->estado = VEHICULO_MANTENIMIENTO;
            cambiado = 1;
        } else if (opcion == 3) {
            v->estado = VEHICULO_BAJA;
            cambiado = 1;
        } else if (opcion != 0) {
            printf("Error: opcion invalida.\n");
        }
    }

    return cambiado;
}

/* ---------- Operaciones ---------- */

/* "seguir" vale 1 mientras no haya habido ningun problema */

void altaVehiculo(void)
{
    Vehiculo nuevo;
<<<<<<< HEAD
    Vehiculo existente;
    long pos;
    int seguir = 1;
=======
    CodigoError error = ERR_OK;
    long posicion;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    int campo;

    mostrarTitulo("ALTA DE VEHICULO");
    memset(&nuevo, 0, sizeof(Vehiculo));

    if (!pedirPatente(nuevo.patente)) {
<<<<<<< HEAD
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        pos = buscarVehiculo(nuevo.patente, &existente);

        if (pos >= 0) {
            printf("Error: ya existe un vehiculo con la patente %s\n", nuevo.patente);
            seguir = 0;
        } else if (pos < -1) {
            printf("Error: no se pudo abrir el archivo de vehiculos.\n");
            seguir = 0;
        }
    }

    for (campo = 1; seguir && campo <= 6; campo++) {
        if (!pedirDato(&nuevo, campo)) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    if (seguir) {
        nuevo.estado = VEHICULO_DISPONIBLE;

        if (agregarRegistro(ARCHIVO_VEHICULOS, &nuevo, sizeof(Vehiculo))) {
            printf("\nVehiculo registrado correctamente.\n");
            mostrarVehiculo(&nuevo);
        } else {
            printf("Error: no se pudo guardar el vehiculo en el archivo.\n");
        }
    }
=======
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        posicion = buscarVehiculo(nuevo.patente, NULL);

        if (posicion >= 0) {
            error = ERR_DUPLICADO;
            printf("Error: ya existe un vehiculo con la patente %s.\n",
                   nuevo.patente);
        } else if (posicion < -1) {
            error = ERR_ARCHIVO_LEER;
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    for (campo = 1; error == ERR_OK && campo <= 6; campo++) {
        if (!pedirDatoVehiculo(&nuevo, campo)) {
            error = ERR_CANCELADO;
        }
    }

    if (error == ERR_OK) {
        nuevo.estado = VEHICULO_DISPONIBLE;
        error = agregarRegistro(ARCHIVO_VEHICULOS, &nuevo, sizeof(Vehiculo));

        if (error == ERR_OK) {
            printf("\nVehiculo registrado correctamente.\n");
            mostrarVehiculo(&nuevo);
        } else {
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
}

void listarVehiculos(void)
{
    FILE *archivo;
    Vehiculo vehiculo;
    int cantidad = 0;

    mostrarTitulo("LISTADO DE VEHICULOS");
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
<<<<<<< HEAD
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
    } else {
        mostrarTabla();

        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            mostrarFila(&vehiculo);
=======
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_VEHICULOS);
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            if (cantidad == 0) {
                imprimirEncabezadoVehiculos();
            }

            imprimirFilaVehiculo(&vehiculo);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
            cantidad++;
        }

        cerrarArchivo(archivo);
<<<<<<< HEAD
        printf("\nTotal de vehiculos: %d\n", cantidad);
=======

        if (cantidad == 0) {
            printf("No hay vehiculos registrados.\n");
        } else {
            printf("\nTotal de vehiculos: %d\n", cantidad);
        }
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

int listarVehiculosDisponibles(void)
{
    FILE *archivo;
    Vehiculo vehiculo;
    int disponibles = 0;

    mostrarTitulo("VEHICULOS DISPONIBLES");
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
<<<<<<< HEAD
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
=======
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_VEHICULOS);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        disponibles = -1;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            if (vehiculo.estado == VEHICULO_DISPONIBLE) {
                if (disponibles == 0) {
<<<<<<< HEAD
                    mostrarTabla();
                }
                mostrarFila(&vehiculo);
=======
                    imprimirEncabezadoVehiculos();
                }

                imprimirFilaVehiculo(&vehiculo);
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
                disponibles++;
            }
        }

        cerrarArchivo(archivo);

        if (disponibles == 0) {
<<<<<<< HEAD
            printf("No hay vehiculos disponibles.\n");
=======
            printf("No hay vehiculos disponibles en este momento.\n");
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
        } else {
            printf("\nVehiculos disponibles: %d\n", disponibles);
        }
    }

    return disponibles;
}

void consultarVehiculo(void)
{
    Vehiculo vehiculo;
    CodigoError error = ERR_OK;
    char patente[12];

    mostrarTitulo("BUSCAR VEHICULO");

    if (!pedirPatente(patente)) {
        error = ERR_CANCELADO;
    } else {
        error = obtenerVehiculo(patente, &vehiculo);

        if (error == ERR_NO_ENCONTRADO) {
            printf("Error: no existe un vehiculo con la patente %s.\n",
                   patente);
        } else if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    if (error == ERR_OK) {
        mostrarVehiculo(&vehiculo);
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
<<<<<<< HEAD
    } else {
        resultado = obtenerVehiculo(patente, &vehiculo);

        if (resultado == 0) {
            printf("Error: no existe un vehiculo con la patente %s\n", patente);
        } else if (resultado == 1) {
            mostrarVehiculo(&vehiculo);
        }
=======
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void modificarVehiculo(void)
{
<<<<<<< HEAD
    Vehiculo vehiculo;
    char patente[12];
    long pos = -1;
    int seguir = 1;
=======
    Vehiculo editado;
    CodigoError error = ERR_OK;
    char patente[12];
    long posicion = -1;
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    int opcion = 0;
    int campo;

    mostrarTitulo("MODIFICAR VEHICULO");

    if (!pedirPatente(patente)) {
<<<<<<< HEAD
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        pos = buscarVehiculo(patente, &vehiculo);

        if (pos == -1) {
            printf("Error: no existe un vehiculo con la patente %s\n", patente);
            seguir = 0;
        } else if (pos < -1) {
            printf("Error: no se pudo abrir el archivo de vehiculos.\n");
            seguir = 0;
        }
    }

    if (seguir) {
        mostrarVehiculo(&vehiculo);
        printf("Que desea modificar?\n");
        printf("1. Marca\n2. Modelo\n3. Anio\n4. Tipo\n5. Capacidad\n");
        printf("6. Precio por dia\n7. Estado\n8. Todo (menos el estado)\n0. Cancelar\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 0) {
            mostrarCancelado();
            seguir = 0;
        } else if (opcion < 1 || opcion > 8) {
            printf("Error: opcion invalida.\n");
            seguir = 0;
        }
    }

    /* se modifica la copia; si el usuario cancela no se toca el archivo */
    if (seguir) {
        if (opcion == 7) {
            if (!pedirNuevoEstado(&vehiculo)) {
                mostrarCancelado();
                seguir = 0;
            }
        } else if (opcion == 8) {
            for (campo = 1; seguir && campo <= 6; campo++) {
                if (!pedirDato(&vehiculo, campo)) {
                    mostrarCancelado();
                    seguir = 0;
                }
            }
        } else if (!pedirDato(&vehiculo, opcion)) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    if (seguir) {
        if (escribirRegistroEn(ARCHIVO_VEHICULOS, &vehiculo, sizeof(Vehiculo), pos)) {
            printf("\nVehiculo modificado correctamente.\n");
            mostrarVehiculo(&vehiculo);
        } else {
            printf("Error: no se pudo guardar el vehiculo en el archivo.\n");
        }
=======
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        posicion = buscarVehiculo(patente, &editado);
        error = errorPorPosicion(posicion);

        if (error == ERR_NO_ENCONTRADO) {
            printf("Error: no existe un vehiculo con la patente %s.\n",
                   patente);
        } else if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    if (error == ERR_OK) {
        printf("\nDatos actuales:");
        mostrarVehiculo(&editado);

        printf("Que desea modificar?\n");
        printf("1. Marca\n");
        printf("2. Modelo\n");
        printf("3. Anio\n");
        printf("4. Tipo de vehiculo\n");
        printf("5. Capacidad\n");
        printf("6. Precio por dia\n");
        printf("7. Estado\n");
        printf("8. Todos los datos (excepto el estado)\n");
        printf("0. Cancelar\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        if (opcion == 0) {
            error = ERR_CANCELADO;
        } else if (opcion < 1 || opcion > 8) {
            error = ERR_DATO_INVALIDO;
            printf("Error: opcion invalida.\n");
        }
    }

    /* Se edita la copia en memoria: si se cancela, el archivo no cambia */
    if (error == ERR_OK) {
        if (opcion == 7) {
            if (!pedirNuevoEstado(&editado)) {
                error = ERR_CANCELADO;
            }
        } else if (opcion == 8) {
            for (campo = 1; error == ERR_OK && campo <= 6; campo++) {
                if (!pedirDatoVehiculo(&editado, campo)) {
                    error = ERR_CANCELADO;
                }
            }
        } else if (!pedirDatoVehiculo(&editado, opcion)) {
            error = ERR_CANCELADO;
        }
    }

    if (error == ERR_OK) {
        error = escribirRegistroEn(ARCHIVO_VEHICULOS, &editado,
                                   sizeof(Vehiculo), posicion);

        if (error == ERR_OK) {
            printf("\nVehiculo modificado correctamente.\n");
            mostrarVehiculo(&editado);
        } else {
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
>>>>>>> cc775f8b4f4a69bdce497048da840de352c14ccd
    }
}

void menuVehiculos(void)
{
    int opcion;

    do {
        printf("\n========== MENU VEHICULOS ==========\n");
        printf("1. Alta de vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Buscar vehiculo por patente\n");
        printf("4. Listar vehiculos\n");
        printf("5. Listar vehiculos disponibles\n");
        printf("0. Volver\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 1) {
            altaVehiculo();
        } else if (opcion == 2) {
            modificarVehiculo();
        } else if (opcion == 3) {
            consultarVehiculo();
        } else if (opcion == 4) {
            listarVehiculos();
        } else if (opcion == 5) {
            listarVehiculosDisponibles();
        } else if (opcion != 0) {
            printf("Opcion invalida.\n");
        }
    } while (opcion != 0);
}
