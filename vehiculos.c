#include <stdio.h>
#include <string.h>

#include "vehiculos.h"
#include "archivos.h"
#include "utiles.h"

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
}

int buscarVehiculoPorPatente(const char patente[])
{
    return (int)buscarVehiculo(patente, NULL);
}

CodigoError obtenerVehiculo(const char patente[], Vehiculo *vehiculo)
{
    return errorPorPosicion(buscarVehiculo(patente, vehiculo));
}

CodigoError actualizarEstadoVehiculo(const char patente[],
                                     EstadoVehiculo estado)
{
    Vehiculo vehiculo;
    long posicion = buscarVehiculo(patente, &vehiculo);
    CodigoError error = errorPorPosicion(posicion);

    if (error == ERR_OK) {
        vehiculo.estado = estado;
        error = escribirRegistroEn(ARCHIVO_VEHICULOS, &vehiculo,
                                   sizeof(Vehiculo), posicion);
    }

    return error;
}

/* =========================================================
   PRESENTACION
   ========================================================= */

static void imprimirEncabezadoVehiculos(void)
{
    printf("%-9s %-14s %-16s %-5s %-12s %-4s %12s  %-16s\n",
           "Patente", "Marca", "Modelo", "Anio", "Tipo", "Cap.",
           "Precio/dia", "Estado");
    printf("---------------------------------------------------------"
           "-------------------------------------\n");
}

static void imprimirFilaVehiculo(const Vehiculo *v)
{
    printf("%-9s %-14.14s %-16.16s %-5d %-12.12s %-4d %12.2f  %-16s\n",
           v->patente, v->marca, v->modelo, v->anio, v->tipo,
           v->capacidad, v->precioDia, textoEstadoVehiculo(v->estado));
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

/* =========================================================
   CARGA DE DATOS
   ========================================================= */

/* Pide un dato del vehiculo segun el numero de campo. Devuelve 0 si cancela */
static int pedirDatoVehiculo(Vehiculo *v, int campo)
{
    int numero;
    float precio;
    int ok = 0;

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
    }

    return ok;
}

/* Cambia el estado manual (mantenimiento / baja / disponible).
   Devuelve 1 si se eligio un estado nuevo. */
static int pedirNuevoEstado(Vehiculo *v)
{
    int opcion;
    int cambiado = 0;

    if (v->estado == VEHICULO_ALQUILADO) {
        printf("Error: el vehiculo esta alquilado. Registre primero la "
               "devolucion para poder cambiar su estado.\n");
    } else {
        printf("\nSeleccione el nuevo estado:\n");
        printf("1. Disponible\n");
        printf("2. En mantenimiento\n");
        printf("3. Baja\n");
        printf("0. Cancelar\n");

        opcion = leerOpcion("Opcion: ");

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

/* =========================================================
   OPERACIONES
   ========================================================= */

void altaVehiculo(void)
{
    Vehiculo nuevo;
    CodigoError error = ERR_OK;
    long posicion;
    int campo;

    mostrarTitulo("ALTA DE VEHICULO");
    memset(&nuevo, 0, sizeof(Vehiculo));

    if (!pedirPatente(nuevo.patente)) {
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
}

void listarVehiculos(void)
{
    FILE *archivo;
    Vehiculo vehiculo;
    int cantidad = 0;

    mostrarTitulo("LISTADO DE VEHICULOS");
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_VEHICULOS);
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            if (cantidad == 0) {
                imprimirEncabezadoVehiculos();
            }

            imprimirFilaVehiculo(&vehiculo);
            cantidad++;
        }

        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay vehiculos registrados.\n");
        } else {
            printf("\nTotal de vehiculos: %d\n", cantidad);
        }
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
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_VEHICULOS);
        disponibles = -1;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            if (vehiculo.estado == VEHICULO_DISPONIBLE) {
                if (disponibles == 0) {
                    imprimirEncabezadoVehiculos();
                }

                imprimirFilaVehiculo(&vehiculo);
                disponibles++;
            }
        }

        cerrarArchivo(archivo);

        if (disponibles == 0) {
            printf("No hay vehiculos disponibles en este momento.\n");
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
    }
}

void modificarVehiculo(void)
{
    Vehiculo editado;
    CodigoError error = ERR_OK;
    char patente[12];
    long posicion = -1;
    int opcion = 0;
    int campo;

    mostrarTitulo("MODIFICAR VEHICULO");

    if (!pedirPatente(patente)) {
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
    }
}

void menuVehiculos(void)
{
    int opcion;

    do {
        printf("\n========================================\n");
        printf("            MENU DE VEHICULOS\n");
        printf("========================================\n");
        printf("1. Alta de vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Buscar vehiculo por patente\n");
        printf("4. Listar todos los vehiculos\n");
        printf("5. Listar vehiculos disponibles\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1: altaVehiculo();                break;
            case 2: modificarVehiculo();           break;
            case 3: consultarVehiculo();           break;
            case 4: listarVehiculos();             break;
            case 5: listarVehiculosDisponibles();  break;
            case 0:                                break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
