#include <stdio.h>
#include <string.h>

#include "vehiculos.h"
#include "archivos.h"
#include "utiles.h"

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
}

int buscarVehiculoPorPatente(const char patente[])
{
    Vehiculo vehiculo;

    return (int)buscarVehiculo(patente, &vehiculo);
}

int obtenerVehiculo(const char patente[], Vehiculo *vehiculo)
{
    long pos = buscarVehiculo(patente, vehiculo);
    int resultado = 1;

    if (pos == -1) {
        resultado = 0;
    } else if (pos < -1) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
        resultado = -1;
    }

    return resultado;
}

int actualizarEstadoVehiculo(const char patente[], EstadoVehiculo estado)
{
    Vehiculo vehiculo;
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
    }

    return ok;
}

/* Cambia el estado a mano. Devuelve 1 si se cambio */
static int pedirNuevoEstado(Vehiculo *v)
{
    int opcion;
    int cambiado = 0;

    if (v->estado == VEHICULO_ALQUILADO) {
        printf("Error: el vehiculo esta alquilado. Primero registre la devolucion.\n");
    } else {
        printf("1. Disponible\n2. En mantenimiento\n3. Baja\n0. Cancelar\n");
        opcion = leerOpcion("Nuevo estado: ");

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
    Vehiculo existente;
    long pos;
    int seguir = 1;
    int campo;

    mostrarTitulo("ALTA DE VEHICULO");
    memset(&nuevo, 0, sizeof(Vehiculo));

    if (!pedirPatente(nuevo.patente)) {
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
}

void listarVehiculos(void)
{
    FILE *archivo;
    Vehiculo vehiculo;
    int cantidad = 0;

    mostrarTitulo("LISTADO DE VEHICULOS");
    archivo = abrirLectura(ARCHIVO_VEHICULOS);

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
    } else {
        mostrarTabla();

        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            mostrarFila(&vehiculo);
            cantidad++;
        }

        cerrarArchivo(archivo);
        printf("\nTotal de vehiculos: %d\n", cantidad);
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
        printf("Error: no se pudo abrir el archivo de vehiculos.\n");
        disponibles = -1;
    } else {
        while (leerSiguiente(archivo, &vehiculo, sizeof(Vehiculo))) {
            if (vehiculo.estado == VEHICULO_DISPONIBLE) {
                if (disponibles == 0) {
                    mostrarTabla();
                }
                mostrarFila(&vehiculo);
                disponibles++;
            }
        }

        cerrarArchivo(archivo);

        if (disponibles == 0) {
            printf("No hay vehiculos disponibles.\n");
        } else {
            printf("\nVehiculos disponibles: %d\n", disponibles);
        }
    }

    return disponibles;
}

void consultarVehiculo(void)
{
    Vehiculo vehiculo;
    char patente[12];
    int resultado;

    mostrarTitulo("BUSCAR VEHICULO");

    if (!pedirPatente(patente)) {
        mostrarCancelado();
    } else {
        resultado = obtenerVehiculo(patente, &vehiculo);

        if (resultado == 0) {
            printf("Error: no existe un vehiculo con la patente %s\n", patente);
        } else if (resultado == 1) {
            mostrarVehiculo(&vehiculo);
        }
    }
}

void modificarVehiculo(void)
{
    Vehiculo vehiculo;
    char patente[12];
    long pos = -1;
    int seguir = 1;
    int opcion = 0;
    int campo;

    mostrarTitulo("MODIFICAR VEHICULO");

    if (!pedirPatente(patente)) {
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
