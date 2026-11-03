#include <stdio.h>
#include <string.h>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "vehiculos.h"
#include "movimientos.h"
#include "utiles.h"

#define MAX_DIAS_ALQUILER 365
#define TOLERANCIA        0.005f

/* =========================================================
   ACCESO A ARCHIVO
   ========================================================= */

long buscarAlquiler(int numero, Alquiler *salida)
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler actual;
    long posicion = -1;
    long indice = 0;

    if (archivo == NULL) {
        posicion = -2;
    } else {
        while (posicion < 0 &&
               leerSiguiente(archivo, &actual, sizeof(Alquiler))) {
            if (actual.numero == numero) {
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

int buscarAlquilerPorNumero(int numero)
{
    return (int)buscarAlquiler(numero, NULL);
}

/* El numero siguiente es el del ultimo registro mas uno (los alquileres
   solo se agregan al final y nunca se eliminan). */
static CodigoError proximoNumeroAlquiler(int *numero)
{
    long cantidad = contarRegistros(ARCHIVO_ALQUILERES, sizeof(Alquiler));
    Alquiler ultimo;
    CodigoError error = ERR_OK;

    if (cantidad < 0) {
        error = ERR_ARCHIVO_LEER;
    } else if (cantidad == 0) {
        *numero = 1;
    } else {
        error = leerRegistroEn(ARCHIVO_ALQUILERES, &ultimo,
                               sizeof(Alquiler), cantidad - 1);

        if (error == ERR_OK) {
            *numero = ultimo.numero + 1;
        }
    }

    return error;
}

int clienteTieneAlquilerActivo(const char dni[])
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    int resultado = 0;

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_ALQUILERES);
        resultado = -1;
    } else {
        while (resultado == 0 &&
               leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.estado == ALQUILER_ACTIVO &&
                strcmp(alquiler.dniCliente, dni) == 0) {
                resultado = 1;
            }
        }

        cerrarArchivo(archivo);
    }

    return resultado;
}

/* =========================================================
   CALCULOS Y PRESENTACION
   ========================================================= */

float saldoAlquiler(const Alquiler *alquiler)
{
    float saldo = 0.0f;

    if (alquiler->estado != ALQUILER_CANCELADO &&
        alquiler->importeTotal - alquiler->importePagado > TOLERANCIA) {
        saldo = alquiler->importeTotal - alquiler->importePagado;
    }

    return saldo;
}

/* El precio pactado se conserva aunque despues se modifique el vehiculo */
static float precioDiarioPactado(const Alquiler *alquiler)
{
    float precio = 0.0f;

    if (alquiler->cantidadDias > 0) {
        precio = alquiler->importeTotal / (float)alquiler->cantidadDias;
    }

    return precio;
}

void imprimirEncabezadoAlquileres(void)
{
    printf("%-5s %-9s %-8s %-10s %-10s %-10s %4s %11s %11s %11s  %-10s\n",
           "Nro", "DNI", "Patente", "Inicio", "Prevista", "Devolucion",
           "Dias", "Total", "Pagado", "Saldo", "Estado");
    printf("---------------------------------------------------------"
           "---------------------------------------------------\n");
}

void imprimirFilaAlquiler(const Alquiler *a)
{
    printf("%-5d %-9s %-8s ", a->numero, a->dniCliente, a->patente);
    imprimirFecha(a->fechaInicio);
    printf(" ");
    imprimirFecha(a->fechaPrevista);
    printf(" ");
    imprimirFecha(a->fechaDevolucion);
    printf(" %4d %11.2f %11.2f %11.2f  %-10s\n",
           a->cantidadDias, a->importeTotal, a->importePagado,
           saldoAlquiler(a), textoEstadoAlquiler(a->estado));
}

/* Registra el movimiento economico asociado a un alquiler */
static CodigoError registrarMovimientoDe(const Alquiler *alquiler,
                                         TipoMovimiento tipo, float monto,
                                         Fecha fecha)
{
    Movimiento movimiento;
    CodigoError error;

    memset(&movimiento, 0, sizeof(Movimiento));
    movimiento.numeroAlquiler = alquiler->numero;
    movimiento.tipo = tipo;
    movimiento.monto = monto;
    movimiento.fecha = fecha;
    strcpy(movimiento.dniCliente, alquiler->dniCliente);
    strcpy(movimiento.patente, alquiler->patente);

    error = registrarMovimiento(movimiento);

    if (error != ERR_OK) {
        printf("ADVERTENCIA: la operacion se guardo pero no pudo "
               "registrarse el movimiento (%s).\n", textoTipoMovimiento(tipo));
    }

    return error;
}

/*
 * Muestra los alquileres activos y pide que se elija uno por su numero.
 * Devuelve ERR_OK (con la posicion y el registro elegido), ERR_SIN_DATOS si
 * no hay activos, ERR_CANCELADO si el usuario cancela o un error de archivo.
 */
static CodigoError seleccionarAlquilerActivo(const char *mensaje,
                                             long *posicion,
                                             Alquiler *seleccionado)
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    CodigoError error = ERR_OK;
    long encontrada;
    int activos = 0;
    int numero;
    int listo = 0;

    if (archivo == NULL) {
        error = ERR_ARCHIVO_ABRIR;
        mostrarError(error, ARCHIVO_ALQUILERES);
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.estado == ALQUILER_ACTIVO) {
                if (activos == 0) {
                    printf("\nAlquileres activos:\n");
                    imprimirEncabezadoAlquileres();
                }

                imprimirFilaAlquiler(&alquiler);
                activos++;
            }
        }

        cerrarArchivo(archivo);

        if (activos == 0) {
            printf("No hay alquileres activos.\n");
            error = ERR_SIN_DATOS;
        }
    }

    if (error == ERR_OK) {
        printf("\n");
    }

    while (error == ERR_OK && !listo) {
        if (!pedirEntero(mensaje, 1, 999999, &numero)) {
            error = ERR_CANCELADO;
        } else {
            encontrada = buscarAlquiler(numero, seleccionado);

            if (encontrada < -1) {
                error = ERR_ARCHIVO_LEER;
                mostrarError(error, ARCHIVO_ALQUILERES);
            } else if (encontrada == -1) {
                printf("Error: no existe un alquiler con el numero %d.\n",
                       numero);
            } else if (seleccionado->estado != ALQUILER_ACTIVO) {
                printf("Error: el alquiler %d esta %s; solo se pueden "
                       "operar alquileres activos.\n", numero,
                       textoEstadoAlquiler(seleccionado->estado));
            } else {
                *posicion = encontrada;
                listo = 1;
            }
        }
    }

    return error;
}

/* =========================================================
   OPERACIONES
   ========================================================= */

void realizarAlquiler(void)
{
    Alquiler nuevo;
    Cliente cliente;
    Vehiculo vehiculo;
    Fecha inicio;
    CodigoError error = ERR_OK;
    CodigoError resultado;
    char dni[12];
    char patente[12];
    int disponibles;
    int dias = 0;
    int listo;
    float importe = 0.0f;

    mostrarTitulo("REALIZAR ALQUILER");

    /* Se informa primero que vehiculos se pueden alquilar */
    disponibles = listarVehiculosDisponibles();

    if (disponibles < 0) {
        error = ERR_ARCHIVO_LEER;
    } else if (disponibles == 0) {
        error = ERR_SIN_DATOS;
    }

    /* Cliente: debe existir y estar activo */
    if (error == ERR_OK) {
        printf("\n");
    }

    listo = 0;

    while (error == ERR_OK && !listo) {
        if (!pedirDNI(dni)) {
            error = ERR_CANCELADO;
        } else {
            resultado = obtenerCliente(dni, &cliente);

            if (resultado == ERR_NO_ENCONTRADO) {
                printf("Error: el cliente no existe. Registrelo primero en "
                       "el menu de clientes.\n");
            } else if (resultado != ERR_OK) {
                error = resultado;
                mostrarError(error, ARCHIVO_CLIENTES);
            } else if (cliente.estado != CLIENTE_ACTIVO) {
                printf("Error: el cliente esta dado de baja.\n");
            } else {
                listo = 1;
            }
        }
    }

    /* Vehiculo: debe existir y estar disponible */
    listo = 0;

    while (error == ERR_OK && !listo) {
        if (!pedirPatente(patente)) {
            error = ERR_CANCELADO;
        } else {
            resultado = obtenerVehiculo(patente, &vehiculo);

            if (resultado == ERR_NO_ENCONTRADO) {
                printf("Error: el vehiculo no existe.\n");
            } else if (resultado != ERR_OK) {
                error = resultado;
                mostrarError(error, ARCHIVO_VEHICULOS);
            } else if (vehiculo.estado != VEHICULO_DISPONIBLE) {
                printf("Error: el vehiculo no esta disponible "
                       "(estado: %s).\n",
                       textoEstadoVehiculo(vehiculo.estado));
            } else {
                listo = 1;
            }
        }
    }

    if (error == ERR_OK && !pedirFecha("Fecha de inicio del alquiler",
                                       &inicio)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK && !pedirEntero("Cantidad de dias", 1,
                                        MAX_DIAS_ALQUILER, &dias)) {
        error = ERR_CANCELADO;
    }

    if (error == ERR_OK) {
        memset(&nuevo, 0, sizeof(Alquiler));
        strcpy(nuevo.dniCliente, dni);
        strcpy(nuevo.patente, patente);
        nuevo.fechaInicio = inicio;
        nuevo.fechaPrevista = sumarDias(inicio, dias);
        nuevo.cantidadDias = dias;
        importe = vehiculo.precioDia * (float)dias;
        nuevo.importeTotal = importe;
        nuevo.importePagado = 0.0f;
        nuevo.estado = ALQUILER_ACTIVO;

        printf("\n--- RESUMEN DEL ALQUILER ---\n");
        printf("Cliente:             %s (%s)\n", cliente.nombre, cliente.dni);
        printf("Vehiculo:            %s %s (%s)\n", vehiculo.marca,
               vehiculo.modelo, vehiculo.patente);
        printf("Fecha de inicio:     ");
        imprimirFecha(nuevo.fechaInicio);
        printf("\nDevolucion prevista: ");
        imprimirFecha(nuevo.fechaPrevista);
        printf("\nCantidad de dias:    %d\n", dias);
        printf("Precio por dia:      $%.2f\n", vehiculo.precioDia);
        printf("Importe total:       $%.2f\n\n", importe);

        if (!confirmar("Confirmar alquiler")) {
            error = ERR_CANCELADO;
        }
    }

    /* Primero se marca el vehiculo y luego se agrega el alquiler; si el
       alta falla, se restaura el estado del vehiculo. */
    if (error == ERR_OK) {
        error = actualizarEstadoVehiculo(patente, VEHICULO_ALQUILADO);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_VEHICULOS);
        }
    }

    if (error == ERR_OK) {
        error = proximoNumeroAlquiler(&nuevo.numero);

        if (error == ERR_OK) {
            error = agregarRegistro(ARCHIVO_ALQUILERES, &nuevo,
                                    sizeof(Alquiler));
        }

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_ALQUILERES);

            if (actualizarEstadoVehiculo(patente, VEHICULO_DISPONIBLE)
                != ERR_OK) {
                printf("ADVERTENCIA: no se pudo restaurar el estado del "
                       "vehiculo %s. Revise los datos.\n", patente);
            }

            printf("El alquiler no fue registrado.\n");
        } else {
            registrarMovimientoDe(&nuevo, MOV_ALQUILER, importe, inicio);
            printf("\nAlquiler registrado correctamente. Numero de "
                   "alquiler: %d\n", nuevo.numero);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void registrarDevolucion(void)
{
    Alquiler alquiler;
    Alquiler anterior;
    Fecha devolucion;
    CodigoError error;
    long posicion = -1;
    int diasReales = 0;
    int diasExtra = 0;
    int listo = 0;
    float precioDia = 0.0f;
    float recargo = 0.0f;
    float importeFinal = 0.0f;
    float saldo;

    mostrarTitulo("REGISTRAR DEVOLUCION");

    error = seleccionarAlquilerActivo("Numero de alquiler a devolver",
                                      &posicion, &alquiler);

    while (error == ERR_OK && !listo) {
        if (!pedirFecha("Fecha real de devolucion", &devolucion)) {
            error = ERR_CANCELADO;
        } else if (compararFechas(devolucion, alquiler.fechaInicio) < 0) {
            printf("Error: la fecha de devolucion no puede ser anterior a "
                   "la fecha de inicio (");
            imprimirFecha(alquiler.fechaInicio);
            printf(").\n");
        } else {
            listo = 1;
        }
    }

    if (error == ERR_OK) {
        /* Se cobran como minimo los dias contratados; los dias que exceden
           la cantidad contratada se facturan al mismo precio pactado. */
        diasReales = diasEntre(alquiler.fechaInicio, devolucion);

        if (diasReales < 1) {
            diasReales = 1;
        }

        if (diasReales > alquiler.cantidadDias) {
            diasExtra = diasReales - alquiler.cantidadDias;
        }

        precioDia = precioDiarioPactado(&alquiler);
        recargo = precioDia * (float)diasExtra;
        importeFinal = alquiler.importeTotal + recargo;
        saldo = importeFinal - alquiler.importePagado;

        if (saldo < TOLERANCIA) {
            saldo = 0.0f;
        }

        printf("\n--- RESUMEN DE LA DEVOLUCION ---\n");
        printf("Alquiler Nro:       %d (%s - %s)\n", alquiler.numero,
               alquiler.dniCliente, alquiler.patente);
        printf("Dias contratados:   %d\n", alquiler.cantidadDias);
        printf("Dias transcurridos: %d\n", diasReales);
        printf("Dias adicionales:   %d\n", diasExtra);
        printf("Precio por dia:     $%.2f\n", precioDia);
        printf("Recargo:            $%.2f\n", recargo);
        printf("Importe final:      $%.2f\n", importeFinal);
        printf("Importe pagado:     $%.2f\n", alquiler.importePagado);
        printf("Saldo pendiente:    $%.2f\n\n", saldo);

        if (!confirmar("Confirmar devolucion")) {
            error = ERR_CANCELADO;
        }
    }

    /* El alquiler se actualiza en su lugar; si falla el vehiculo, se
       vuelve a escribir el registro anterior. */
    if (error == ERR_OK) {
        anterior = alquiler;
        alquiler.fechaDevolucion = devolucion;
        alquiler.cantidadDias = alquiler.cantidadDias + diasExtra;
        alquiler.importeTotal = importeFinal;
        alquiler.estado = ALQUILER_FINALIZADO;

        error = escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler,
                                   sizeof(Alquiler), posicion);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_ALQUILERES);
        }
    }

    if (error == ERR_OK) {
        error = actualizarEstadoVehiculo(alquiler.patente,
                                         VEHICULO_DISPONIBLE);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_VEHICULOS);

            if (escribirRegistroEn(ARCHIVO_ALQUILERES, &anterior,
                                   sizeof(Alquiler), posicion) != ERR_OK) {
                printf("ADVERTENCIA: no se pudo deshacer la devolucion. "
                       "Revise los datos.\n");
            }

            printf("La devolucion no fue registrada.\n");
        }
    }

    if (error == ERR_OK) {
        if (diasExtra > 0) {
            registrarMovimientoDe(&alquiler, MOV_AJUSTE, recargo, devolucion);
        }

        registrarMovimientoDe(&alquiler, MOV_DEVOLUCION, importeFinal,
                              devolucion);

        printf("\nDevolucion registrada correctamente.\n");

        if (saldoAlquiler(&alquiler) > 0) {
            printf("El cliente adeuda $%.2f. Puede registrar el pago desde "
                   "el menu de pagos.\n", saldoAlquiler(&alquiler));
        }
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void modificarAlquiler(void)
{
    Alquiler alquiler;
    Fecha fecha;
    CodigoError error;
    long posicion = -1;
    int nuevosDias = 0;
    float precioDia = 0.0f;
    float nuevoImporte = 0.0f;
    float diferencia = 0.0f;

    mostrarTitulo("MODIFICAR ALQUILER");

    error = seleccionarAlquilerActivo("Numero de alquiler a modificar",
                                      &posicion, &alquiler);

    if (error == ERR_OK) {
        precioDia = precioDiarioPactado(&alquiler);

        printf("\nSe puede modificar la duracion del alquiler "
               "(actual: %d dias, $%.2f por dia).\n",
               alquiler.cantidadDias, precioDia);

        if (!pedirEntero("Nueva cantidad de dias", 1, MAX_DIAS_ALQUILER,
                         &nuevosDias)) {
            error = ERR_CANCELADO;
        } else if (nuevosDias == alquiler.cantidadDias) {
            printf("La cantidad de dias es la misma. No hay cambios.\n");
            error = ERR_DATO_INVALIDO;
        }
    }

    if (error == ERR_OK) {
        nuevoImporte = precioDia * (float)nuevosDias;

        if (nuevoImporte + TOLERANCIA < alquiler.importePagado) {
            error = ERR_ESTADO_INVALIDO;
            printf("Error: el nuevo importe ($%.2f) es menor a lo ya pagado "
                   "($%.2f). Elija mas dias o cancele el alquiler.\n",
                   nuevoImporte, alquiler.importePagado);
        }
    }

    if (error == ERR_OK) {
        diferencia = nuevoImporte - alquiler.importeTotal;

        printf("\nNuevo importe total: $%.2f (diferencia: %+.2f)\n",
               nuevoImporte, diferencia);

        if (!pedirFechaOperacion("Fecha de la modificacion", &fecha) ||
            !confirmar("Confirmar modificacion")) {
            error = ERR_CANCELADO;
        }
    }

    if (error == ERR_OK) {
        alquiler.cantidadDias = nuevosDias;
        alquiler.fechaPrevista = sumarDias(alquiler.fechaInicio, nuevosDias);
        alquiler.importeTotal = nuevoImporte;

        error = escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler,
                                   sizeof(Alquiler), posicion);

        if (error == ERR_OK) {
            registrarMovimientoDe(&alquiler, MOV_AJUSTE, diferencia, fecha);
            printf("\nAlquiler modificado correctamente. Nueva devolucion "
                   "prevista: ");
            imprimirFecha(alquiler.fechaPrevista);
            printf("\n");
        } else {
            mostrarError(error, ARCHIVO_ALQUILERES);
        }
    }

    if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void cancelarAlquiler(void)
{
    Alquiler alquiler;
    Alquiler anterior;
    Fecha fecha;
    CodigoError error;
    long posicion = -1;
    float reembolso = 0.0f;

    mostrarTitulo("CANCELAR ALQUILER");

    error = seleccionarAlquilerActivo("Numero de alquiler a cancelar",
                                      &posicion, &alquiler);

    if (error == ERR_OK) {
        reembolso = alquiler.importePagado;

        printf("\n--- RESUMEN DE LA CANCELACION ---\n");
        printf("Alquiler Nro:     %d (%s - %s)\n", alquiler.numero,
               alquiler.dniCliente, alquiler.patente);
        printf("Dias contratados: %d\n", alquiler.cantidadDias);
        printf("Importe total:    $%.2f\n", alquiler.importeTotal);
        printf("Importe pagado:   $%.2f\n", alquiler.importePagado);

        if (reembolso > TOLERANCIA) {
            printf("CLIENTE - DEVOLUCION: se le devuelve $%.2f (todo lo que "
                   "habia pagado).\n\n", reembolso);
        } else {
            printf("Cliente - devolucion: no corresponde (no hay pagos "
                   "registrados).\n\n");
        }

        if (!pedirFechaOperacion("Fecha de la cancelacion", &fecha) ||
            !confirmar("Confirmar la cancelacion del alquiler")) {
            error = ERR_CANCELADO;
        }
    }

    if (error == ERR_OK) {
        anterior = alquiler;
        alquiler.estado = ALQUILER_CANCELADO;
        alquiler.importePagado = 0.0f;

        error = escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler,
                                   sizeof(Alquiler), posicion);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_ALQUILERES);
        }
    }

    if (error == ERR_OK) {
        error = actualizarEstadoVehiculo(alquiler.patente,
                                         VEHICULO_DISPONIBLE);

        if (error != ERR_OK) {
            mostrarError(error, ARCHIVO_VEHICULOS);

            if (escribirRegistroEn(ARCHIVO_ALQUILERES, &anterior,
                                   sizeof(Alquiler), posicion) != ERR_OK) {
                printf("ADVERTENCIA: no se pudo deshacer la cancelacion. "
                       "Revise los datos.\n");
            }

            printf("El alquiler no fue cancelado.\n");
        }
    }

    if (error == ERR_OK) {
        printf("\nAlquiler cancelado correctamente. El vehiculo %s vuelve a "
               "estar disponible.\n", alquiler.patente);

        if (reembolso > TOLERANCIA) {
            if (registrarMovimientoDe(&alquiler, MOV_AJUSTE, -reembolso,
                                      fecha) == ERR_OK) {
                printf("CLIENTE - DEVOLUCION registrada: $%.2f a favor del "
                       "cliente (DNI %s). Movimiento AJUSTE de -$%.2f.\n",
                       reembolso, alquiler.dniCliente, reembolso);
            } else {
                printf("ATENCION: debe devolver $%.2f al cliente (DNI %s), "
                       "pero la devolucion no quedo registrada.\n",
                       reembolso, alquiler.dniCliente);
            }
        }
    } else if (error == ERR_CANCELADO) {
        mostrarCancelado();
    }
}

void listarAlquileres(void)
{
    FILE *archivo;
    Alquiler alquiler;
    int cantidad = 0;

    mostrarTitulo("LISTADO DE ALQUILERES");
    archivo = abrirLectura(ARCHIVO_ALQUILERES);

    if (archivo == NULL) {
        mostrarError(ERR_ARCHIVO_ABRIR, ARCHIVO_ALQUILERES);
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (cantidad == 0) {
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquiler);
            cantidad++;
        }

        cerrarArchivo(archivo);

        if (cantidad == 0) {
            printf("No hay alquileres registrados.\n");
        } else {
            printf("\nTotal de alquileres: %d\n", cantidad);
        }
    }
}

void menuAlquileres(void)
{
    int opcion;

    do {
        printf("\n========================================\n");
        printf("         GESTION DE ALQUILERES\n");
        printf("========================================\n");
        printf("1. Realizar alquiler\n");
        printf("2. Registrar devolucion\n");
        printf("3. Modificar alquiler\n");
        printf("4. Cancelar alquiler\n");
        printf("5. Listar alquileres\n");
        printf("0. Volver al menu principal\n");
        printf("========================================\n");

        opcion = leerOpcion("Seleccione una opcion: ");

        switch (opcion) {
            case 1: realizarAlquiler();     break;
            case 2: registrarDevolucion();  break;
            case 3: modificarAlquiler();    break;
            case 4: cancelarAlquiler();     break;
            case 5: listarAlquileres();     break;
            case 0:                         break;
            default: printf("Opcion invalida.\n");
        }

    } while (opcion != 0);
}
