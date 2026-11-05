#include <stdio.h>
#include <string.h>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "vehiculos.h"
#include "movimientos.h"
#include "utiles.h"

/* ---------- Archivo ---------- */

long buscarAlquiler(int numero, Alquiler *encontrado)
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    long pos = -1;
    long i = 0;

    if (archivo == NULL) {
        pos = -2;
    } else {
        while (pos == -1 && leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.numero == numero) {
                pos = i;
                *encontrado = alquiler;
            }
            i++;
        }
        cerrarArchivo(archivo);
    }

    return pos;
}

int buscarAlquilerPorNumero(int numero)
{
    Alquiler alquiler;

    return (int)buscarAlquiler(numero, &alquiler);
}

/* El numero nuevo es el del ultimo alquiler del archivo mas uno.
   Devuelve 1 si pudo calcularlo */
static int proximoNumero(int *numero)
{
    long cantidad = contarRegistros(ARCHIVO_ALQUILERES, sizeof(Alquiler));
    Alquiler ultimo;
    int ok = 1;

    if (cantidad < 0) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
        ok = 0;
    } else if (cantidad == 0) {
        *numero = 1;
    } else if (leerRegistroEn(ARCHIVO_ALQUILERES, &ultimo, sizeof(Alquiler), cantidad - 1)) {
        *numero = ultimo.numero + 1;
    } else {
        printf("Error: no se pudo leer el ultimo alquiler del archivo.\n");
        ok = 0;
    }

    return ok;
}

int clienteTieneAlquilerActivo(const char dni[])
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    int resultado = 0;

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
        resultado = -1;
    } else {
        while (resultado == 0 && leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.estado == ALQUILER_ACTIVO && strcmp(alquiler.dniCliente, dni) == 0) {
                resultado = 1;
            }
        }
        cerrarArchivo(archivo);
    }

    return resultado;
}

/* Devuelve 1 si el vehiculo ya tiene un alquiler con fecha posterior a "hoy".
   Sirve para que no se registre un alquiler "en el pasado" de un vehiculo.
   Devuelve -1 si no se pudo abrir el archivo. */
static int vehiculoTieneFechaPosterior(const char patente[], Fecha hoy)
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    Fecha ultima;
    int resultado = 0;

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
        resultado = -1;
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (strcmp(alquiler.patente, patente) == 0 && alquiler.estado != ALQUILER_CANCELADO) {
                ultima = alquiler.fechaInicio;

                if (alquiler.estado == ALQUILER_FINALIZADO) {
                    ultima = alquiler.fechaDevolucion;
                }

                if (compararFechas(ultima, hoy) > 0) {
                    resultado = 1;
                }
            }
        }
        cerrarArchivo(archivo);
    }

    return resultado;
}

/* ---------- Calculos y pantalla ---------- */

float saldoAlquiler(const Alquiler *alquiler)
{
    float saldo = 0;

    if (alquiler->estado != ALQUILER_CANCELADO && alquiler->importeTotal - alquiler->importePagado > 0.005) {
        saldo = alquiler->importeTotal - alquiler->importePagado;
    }

    return saldo;
}

int fechaMaquinaCorrecta(const Alquiler *alquiler, Fecha hoy)
{
    int correcta = 1;

    if (compararFechas(hoy, alquiler->fechaInicio) < 0) {
        correcta = 0;
        printf("Error: la fecha de la maquina (");
        imprimirFecha(hoy);
        printf(") es anterior al inicio del alquiler (");
        imprimirFecha(alquiler->fechaInicio);
        printf("). Revise la fecha del sistema.\n");
    }

    return correcta;
}

void mostrarTablaAlquileres(void)
{
    printf("%-5s %-9s %-8s %-10s %-10s %-10s %4s %11s %11s %11s  %-10s\n",
           "Nro", "DNI", "Patente", "Inicio", "Prevista", "Devolucion",
           "Dias", "Total", "Pagado", "Saldo", "Estado");
}

void mostrarFilaAlquiler(const Alquiler *a)
{
    printf("%-5d %-9s %-8s ", a->numero, a->dniCliente, a->patente);
    imprimirFecha(a->fechaInicio);
    printf(" ");
    imprimirFecha(a->fechaPrevista);
    printf(" ");
    imprimirFecha(a->fechaDevolucion);
    printf(" %4d %11.2f %11.2f %11.2f  %-10s\n", a->cantidadDias,
           a->importeTotal, a->importePagado, saldoAlquiler(a),
           textoEstadoAlquiler(a->estado));
}

/* Guarda un movimiento del alquiler. Devuelve 1 si se pudo guardar */
static int guardarMovimiento(const Alquiler *alquiler, TipoMovimiento tipo, float monto, Fecha fecha)
{
    Movimiento movimiento;
    int ok;

    memset(&movimiento, 0, sizeof(Movimiento));
    movimiento.numeroAlquiler = alquiler->numero;
    movimiento.tipo = tipo;
    movimiento.monto = monto;
    movimiento.fecha = fecha;
    strcpy(movimiento.dniCliente, alquiler->dniCliente);
    strcpy(movimiento.patente, alquiler->patente);

    ok = registrarMovimiento(movimiento);

    if (!ok) {
        printf("ADVERTENCIA: no se pudo registrar el movimiento (%s).\n", textoTipoMovimiento(tipo));
    }

    return ok;
}

/* Muestra los alquileres activos y pide el numero de uno.
   Devuelve 1 si se eligio uno (con su posicion y datos). Si no, ya mostro el motivo. */
static int elegirAlquilerActivo(const char *mensaje, long *pos, Alquiler *elegido)
{
    FILE *archivo = abrirLectura(ARCHIVO_ALQUILERES);
    Alquiler alquiler;
    long encontrado;
    int seguir = 1;
    int activos = 0;
    int numero;
    int listo = 0;

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
        seguir = 0;
    } else {
        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            if (alquiler.estado == ALQUILER_ACTIVO) {
                if (activos == 0) {
                    printf("\nAlquileres activos:\n");
                    mostrarTablaAlquileres();
                }
                mostrarFilaAlquiler(&alquiler);
                activos++;
            }
        }
        cerrarArchivo(archivo);

        if (activos == 0) {
            printf("No hay alquileres activos.\n");
            seguir = 0;
        }
    }

    while (seguir && !listo) {
        if (!pedirEntero(mensaje, 1, 999999, &numero)) {
            mostrarCancelado();
            seguir = 0;
        } else {
            encontrado = buscarAlquiler(numero, elegido);

            if (encontrado < -1) {
                printf("Error: no se pudo abrir el archivo de alquileres.\n");
                seguir = 0;
            } else if (encontrado == -1) {
                printf("Error: no existe el alquiler %d\n", numero);
            } else if (elegido->estado != ALQUILER_ACTIVO) {
                printf("Error: el alquiler %d no esta activo (esta %s).\n", numero,
                       textoEstadoAlquiler(elegido->estado));
            } else {
                *pos = encontrado;
                listo = 1;
            }
        }
    }

    return listo;
}

/* ---------- Operaciones ---------- */

/* En las operaciones, "seguir" vale 1 mientras no haya habido ningun problema.
   Cuando algo falla se muestra un mensaje y seguir pasa a 0. */

void realizarAlquiler(void)
{
    Alquiler nuevo;
    Cliente cliente;
    Vehiculo vehiculo;
    Fecha hoy;
    char dni[12];
    char patente[12];
    int seguir = 1;
    int resultado;
    int disponibles;
    int dias = 0;
    int listo;

    mostrarTitulo("REALIZAR ALQUILER");
    hoy = fechaActual();

    disponibles = listarVehiculosDisponibles();

    if (disponibles <= 0) {
        seguir = 0;
    }

    /* cliente: tiene que existir y estar activo */
    listo = 0;
    while (seguir && !listo) {
        if (!pedirDNI(dni)) {
            mostrarCancelado();
            seguir = 0;
        } else {
            resultado = obtenerCliente(dni, &cliente);

            if (resultado == -1) {
                seguir = 0;
            } else if (resultado == 0) {
                printf("Error: el cliente no existe. Primero debe registrarlo.\n");
            } else if (cliente.estado != CLIENTE_ACTIVO) {
                printf("Error: el cliente esta dado de baja.\n");
            } else {
                listo = 1;
            }
        }
    }

    /* vehiculo: tiene que existir y estar disponible */
    listo = 0;
    while (seguir && !listo) {
        if (!pedirPatente(patente)) {
            mostrarCancelado();
            seguir = 0;
        } else {
            resultado = obtenerVehiculo(patente, &vehiculo);

            if (resultado == -1) {
                seguir = 0;
            } else if (resultado == 0) {
                printf("Error: el vehiculo no existe.\n");
            } else if (vehiculo.estado != VEHICULO_DISPONIBLE) {
                printf("Error: el vehiculo no esta disponible (esta %s).\n",
                       textoEstadoVehiculo(vehiculo.estado));
            } else {
                listo = 1;
            }
        }
    }

    /* la fecha de hoy no puede ser anterior a un alquiler ya registrado del vehiculo */
    if (seguir) {
        resultado = vehiculoTieneFechaPosterior(patente, hoy);

        if (resultado == 1) {
            printf("Error: ese vehiculo tiene un alquiler registrado con fecha posterior a hoy.\n");
            printf("Revise la fecha de la maquina.\n");
        }

        if (resultado != 0) {
            seguir = 0;
        }
    }

    if (seguir && !pedirEntero("Cantidad de dias", 1, 365, &dias)) {
        mostrarCancelado();
        seguir = 0;
    }

    if (seguir) {
        memset(&nuevo, 0, sizeof(Alquiler));
        strcpy(nuevo.dniCliente, dni);
        strcpy(nuevo.patente, patente);
        nuevo.fechaInicio = hoy;
        nuevo.fechaPrevista = sumarDias(hoy, dias);
        nuevo.cantidadDias = dias;
        nuevo.importeTotal = vehiculo.precioDia * dias;
        nuevo.importePagado = 0;
        nuevo.estado = ALQUILER_ACTIVO;

        printf("\n--- RESUMEN ---\n");
        printf("Cliente:             %s (%s)\n", cliente.nombre, cliente.dni);
        printf("Vehiculo:            %s %s (%s)\n", vehiculo.marca, vehiculo.modelo, vehiculo.patente);
        printf("Fecha de inicio:     ");
        imprimirFecha(nuevo.fechaInicio);
        printf("\nDevolucion prevista: ");
        imprimirFecha(nuevo.fechaPrevista);
        printf("\nDias:                %d\n", dias);
        printf("Precio por dia:      $%.2f\n", vehiculo.precioDia);
        printf("Importe total:       $%.2f\n\n", nuevo.importeTotal);

        if (!confirmar("Confirmar el alquiler")) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    /* primero se marca el vehiculo y despues se agrega el alquiler.
       Si falla el alta, el vehiculo vuelve a quedar disponible. */
    if (seguir && !actualizarEstadoVehiculo(patente, VEHICULO_ALQUILADO)) {
        seguir = 0;
    }

    if (seguir) {
        if (!proximoNumero(&nuevo.numero) ||
            !agregarRegistro(ARCHIVO_ALQUILERES, &nuevo, sizeof(Alquiler))) {
            printf("Error: no se pudo guardar el alquiler en el archivo.\n");

            if (!actualizarEstadoVehiculo(patente, VEHICULO_DISPONIBLE)) {
                printf("ADVERTENCIA: no se pudo dejar el vehiculo %s como disponible.\n", patente);
            }
            printf("El alquiler no se registro.\n");
        } else {
            guardarMovimiento(&nuevo, MOV_ALQUILER, nuevo.importeTotal, hoy);
            printf("\nAlquiler registrado. Numero de alquiler: %d\n", nuevo.numero);
        }
    }
}

void registrarDevolucion(void)
{
    Alquiler alquiler;
    Alquiler anterior;
    Fecha hoy;
    long pos = -1;
    int seguir;
    int diasReales = 0;
    int diasExtra = 0;
    float precioDia = 0;
    float recargo = 0;
    float importeFinal = 0;
    float saldo;

    mostrarTitulo("REGISTRAR DEVOLUCION");
    hoy = fechaActual();

    seguir = elegirAlquilerActivo("Numero de alquiler a devolver", &pos, &alquiler);

    if (seguir && !fechaMaquinaCorrecta(&alquiler, hoy)) {
        seguir = 0;
    }

    if (seguir) {
        /* la fecha de devolucion es la de hoy */
        diasReales = diasEntre(alquiler.fechaInicio, hoy);

        if (diasReales < 1) {
            diasReales = 1;
        }

        /* se cobran los dias contratados y, si se paso, los dias de mas */
        if (diasReales > alquiler.cantidadDias) {
            diasExtra = diasReales - alquiler.cantidadDias;
        }

        precioDia = alquiler.importeTotal / alquiler.cantidadDias;
        recargo = precioDia * diasExtra;
        importeFinal = alquiler.importeTotal + recargo;
        saldo = importeFinal - alquiler.importePagado;

        if (saldo < 0.005) {
            saldo = 0;
        }

        printf("\n--- RESUMEN DE LA DEVOLUCION ---\n");
        printf("Alquiler Nro:        %d (%s - %s)\n", alquiler.numero, alquiler.dniCliente, alquiler.patente);
        printf("Fecha de devolucion: ");
        imprimirFecha(hoy);
        printf("\nDias contratados:    %d\n", alquiler.cantidadDias);
        printf("Dias transcurridos:  %d\n", diasReales);
        printf("Dias de mas:         %d\n", diasExtra);
        printf("Precio por dia:      $%.2f\n", precioDia);
        printf("Recargo:             $%.2f\n", recargo);
        printf("Importe final:       $%.2f\n", importeFinal);
        printf("Importe pagado:      $%.2f\n", alquiler.importePagado);
        printf("Saldo pendiente:     $%.2f\n\n", saldo);

        if (!confirmar("Confirmar la devolucion")) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    /* el alquiler se modifica directamente en el archivo.
       Si despues falla el vehiculo, se vuelve a escribir el alquiler anterior. */
    if (seguir) {
        anterior = alquiler;
        alquiler.fechaDevolucion = hoy;
        alquiler.cantidadDias = alquiler.cantidadDias + diasExtra;
        alquiler.importeTotal = importeFinal;
        alquiler.estado = ALQUILER_FINALIZADO;

        if (!escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler, sizeof(Alquiler), pos)) {
            printf("Error: no se pudo guardar el alquiler en el archivo.\n");
            seguir = 0;
        }
    }

    if (seguir && !actualizarEstadoVehiculo(alquiler.patente, VEHICULO_DISPONIBLE)) {
        if (!escribirRegistroEn(ARCHIVO_ALQUILERES, &anterior, sizeof(Alquiler), pos)) {
            printf("ADVERTENCIA: no se pudo deshacer el cambio del alquiler.\n");
        }
        printf("La devolucion no se registro.\n");
        seguir = 0;
    }

    if (seguir) {
        if (diasExtra > 0) {
            guardarMovimiento(&alquiler, MOV_AJUSTE, recargo, hoy);
        }
        guardarMovimiento(&alquiler, MOV_DEVOLUCION, importeFinal, hoy);

        printf("\nDevolucion registrada correctamente.\n");

        if (saldoAlquiler(&alquiler) > 0) {
            printf("El cliente debe $%.2f. Puede pagarlo desde el menu de pagos.\n", saldoAlquiler(&alquiler));
        }
    }
}

void modificarAlquiler(void)
{
    Alquiler alquiler;
    Fecha hoy;
    long pos = -1;
    int seguir;
    int nuevosDias = 0;
    float precioDia = 0;
    float nuevoImporte = 0;
    float diferencia = 0;

    mostrarTitulo("MODIFICAR ALQUILER");
    hoy = fechaActual();

    seguir = elegirAlquilerActivo("Numero de alquiler a modificar", &pos, &alquiler);

    if (seguir && !fechaMaquinaCorrecta(&alquiler, hoy)) {
        seguir = 0;
    }

    if (seguir) {
        precioDia = alquiler.importeTotal / alquiler.cantidadDias;
        printf("\nDias actuales: %d (a $%.2f por dia)\n", alquiler.cantidadDias, precioDia);

        if (!pedirEntero("Nueva cantidad de dias", 1, 365, &nuevosDias)) {
            mostrarCancelado();
            seguir = 0;
        } else if (nuevosDias == alquiler.cantidadDias) {
            printf("Es la misma cantidad de dias. No hay cambios.\n");
            seguir = 0;
        }
    }

    if (seguir) {
        nuevoImporte = precioDia * nuevosDias;

        if (nuevoImporte + 0.005 < alquiler.importePagado) {
            printf("Error: el nuevo importe ($%.2f) es menor a lo que ya pago el cliente ($%.2f).\n",
                   nuevoImporte, alquiler.importePagado);
            printf("Elija mas dias o cancele el alquiler.\n");
            seguir = 0;
        }
    }

    if (seguir) {
        diferencia = nuevoImporte - alquiler.importeTotal;
        printf("Nuevo importe total: $%.2f (diferencia: %.2f)\n", nuevoImporte, diferencia);

        if (!confirmar("Confirmar la modificacion")) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    if (seguir) {
        alquiler.cantidadDias = nuevosDias;
        alquiler.fechaPrevista = sumarDias(alquiler.fechaInicio, nuevosDias);
        alquiler.importeTotal = nuevoImporte;

        if (escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler, sizeof(Alquiler), pos)) {
            guardarMovimiento(&alquiler, MOV_AJUSTE, diferencia, hoy);
            printf("\nAlquiler modificado. Nueva devolucion prevista: ");
            imprimirFecha(alquiler.fechaPrevista);
            printf("\n");
        } else {
            printf("Error: no se pudo guardar el alquiler en el archivo.\n");
        }
    }
}

void cancelarAlquiler(void)
{
    Alquiler alquiler;
    Alquiler anterior;
    Fecha hoy;
    long pos = -1;
    int seguir;
    float devolucion = 0;

    mostrarTitulo("CANCELAR ALQUILER");
    hoy = fechaActual();

    seguir = elegirAlquilerActivo("Numero de alquiler a cancelar", &pos, &alquiler);

    if (seguir && !fechaMaquinaCorrecta(&alquiler, hoy)) {
        seguir = 0;
    }

    if (seguir) {
        devolucion = alquiler.importePagado;

        printf("\n--- RESUMEN DE LA CANCELACION ---\n");
        printf("Alquiler Nro:   %d (%s - %s)\n", alquiler.numero, alquiler.dniCliente, alquiler.patente);
        printf("Importe total:  $%.2f\n", alquiler.importeTotal);
        printf("Importe pagado: $%.2f\n", alquiler.importePagado);

        if (devolucion > 0.005) {
            printf("CLIENTE - DEVOLUCION: se le devuelve $%.2f (todo lo que habia pagado).\n\n", devolucion);
        } else {
            printf("Cliente - devolucion: no corresponde (no hay pagos registrados).\n\n");
        }

        if (!confirmar("Confirmar la cancelacion")) {
            mostrarCancelado();
            seguir = 0;
        }
    }

    if (seguir) {
        anterior = alquiler;
        alquiler.estado = ALQUILER_CANCELADO;
        alquiler.importePagado = 0;

        if (!escribirRegistroEn(ARCHIVO_ALQUILERES, &alquiler, sizeof(Alquiler), pos)) {
            printf("Error: no se pudo guardar el alquiler en el archivo.\n");
            seguir = 0;
        }
    }

    if (seguir && !actualizarEstadoVehiculo(alquiler.patente, VEHICULO_DISPONIBLE)) {
        if (!escribirRegistroEn(ARCHIVO_ALQUILERES, &anterior, sizeof(Alquiler), pos)) {
            printf("ADVERTENCIA: no se pudo deshacer el cambio del alquiler.\n");
        }
        printf("El alquiler no se cancelo.\n");
        seguir = 0;
    }

    if (seguir) {
        printf("\nAlquiler cancelado. El vehiculo %s vuelve a estar disponible.\n", alquiler.patente);

        if (devolucion > 0.005) {
            if (guardarMovimiento(&alquiler, MOV_AJUSTE, -devolucion, hoy)) {
                printf("CLIENTE - DEVOLUCION registrada: $%.2f a favor del cliente (DNI %s).\n",
                       devolucion, alquiler.dniCliente);
            } else {
                printf("ATENCION: hay que devolver $%.2f al cliente (DNI %s) pero no quedo registrado.\n",
                       devolucion, alquiler.dniCliente);
            }
        }
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
        printf("Error: no se pudo abrir el archivo de alquileres.\n");
    } else {
        mostrarTablaAlquileres();

        while (leerSiguiente(archivo, &alquiler, sizeof(Alquiler))) {
            mostrarFilaAlquiler(&alquiler);
            cantidad++;
        }

        cerrarArchivo(archivo);
        printf("\nTotal de alquileres: %d\n", cantidad);
    }
}

void menuAlquileres(void)
{
    int opcion;

    do {
        printf("\n========== MENU ALQUILERES ==========\n");
        printf("1. Realizar alquiler\n");
        printf("2. Registrar devolucion\n");
        printf("3. Modificar alquiler\n");
        printf("4. Cancelar alquiler\n");
        printf("5. Listar alquileres\n");
        printf("0. Volver\n");
        opcion = leerOpcion("Opcion: ");

        if (opcion == 1) {
            realizarAlquiler();
        } else if (opcion == 2) {
            registrarDevolucion();
        } else if (opcion == 3) {
            modificarAlquiler();
        } else if (opcion == 4) {
            cancelarAlquiler();
        } else if (opcion == 5) {
            listarAlquileres();
        } else if (opcion != 0) {
            printf("Opcion invalida.\n");
        }
    } while (opcion != 0);
}
