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

static int cargarAlquileres(Alquiler alquileres[], int *cantidad)
{
    int resultado = leerRegistros(ARCHIVO_ALQUILERES, alquileres,
                                  sizeof(Alquiler), MAX_ALQUILERES);

    if (resultado < 0) {
        printf("Error: no se pudo leer el archivo de alquileres (%s).\n",
               ARCHIVO_ALQUILERES);
        return 0;
    }

    *cantidad = resultado;
    return 1;
}

static int guardarAlquileres(const Alquiler alquileres[], int cantidad)
{
    if (!guardarRegistros(ARCHIVO_ALQUILERES, alquileres,
                          sizeof(Alquiler), cantidad)) {
        printf("Error: no se pudo guardar el archivo de alquileres.\n");
        return 0;
    }

    return 1;
}

static int indiceAlquiler(const Alquiler alquileres[], int cantidad,
                          int numero)
{
    int i;

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].numero == numero) {
            return i;
        }
    }

    return -1;
}

static int nuevoNumero(const Alquiler alquileres[], int cantidad)
{
    int i;
    int mayor = 0;

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].numero > mayor) {
            mayor = alquileres[i].numero;
        }
    }

    return mayor + 1;
}

int buscarAlquilerPorNumero(int numero)
{
    Alquiler alquileres[MAX_ALQUILERES];
    int cantidad;

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return -1;
    }

    return indiceAlquiler(alquileres, cantidad, numero);
}

int clienteTieneAlquilerActivo(const char dni[])
{
    Alquiler alquileres[MAX_ALQUILERES];
    int cantidad;
    int i;

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return -1;
    }

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].estado == ALQUILER_ACTIVO &&
            strcmp(alquileres[i].dniCliente, dni) == 0) {
            return 1;
        }
    }

    return 0;
}

/* =========================================================
   CALCULOS Y PRESENTACION
   ========================================================= */

float saldoAlquiler(const Alquiler *alquiler)
{
    float saldo;

    if (alquiler->estado == ALQUILER_CANCELADO) {
        return 0.0f;
    }

    saldo = alquiler->importeTotal - alquiler->importePagado;

    return saldo > TOLERANCIA ? saldo : 0.0f;
}

/* El precio pactado se conserva aunque despues se modifique el vehiculo */
static float precioDiarioPactado(const Alquiler *alquiler)
{
    if (alquiler->cantidadDias > 0) {
        return alquiler->importeTotal / (float)alquiler->cantidadDias;
    }

    return 0.0f;
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
static int registrarMovimientoDe(const Alquiler *alquiler, TipoMovimiento tipo,
                                 float monto, Fecha fecha)
{
    Movimiento movimiento;

    memset(&movimiento, 0, sizeof(Movimiento));
    movimiento.numeroAlquiler = alquiler->numero;
    movimiento.tipo = tipo;
    movimiento.monto = monto;
    movimiento.fecha = fecha;
    strcpy(movimiento.dniCliente, alquiler->dniCliente);
    strcpy(movimiento.patente, alquiler->patente);

    if (!registrarMovimiento(movimiento)) {
        printf("ADVERTENCIA: la operacion se guardo pero no pudo "
               "registrarse el movimiento (%s).\n", textoTipoMovimiento(tipo));
        return 0;
    }

    return 1;
}

/*
 * Muestra los alquileres activos y pide que se elija uno por su numero.
 * Devuelve la posicion elegida o -1 si no hay activos o el usuario cancela.
 */
static int seleccionarAlquilerActivo(const Alquiler alquileres[], int cantidad,
                                     const char *mensaje)
{
    int i;
    int activos = 0;
    int numero;
    int indice;

    for (i = 0; i < cantidad; i++) {
        if (alquileres[i].estado == ALQUILER_ACTIVO) {
            if (activos == 0) {
                printf("\nAlquileres activos:\n");
                imprimirEncabezadoAlquileres();
            }

            imprimirFilaAlquiler(&alquileres[i]);
            activos++;
        }
    }

    if (activos == 0) {
        printf("No hay alquileres activos.\n");
        return -1;
    }

    printf("\n");

    while (pedirEntero(mensaje, 1, 999999, &numero)) {
        indice = indiceAlquiler(alquileres, cantidad, numero);

        if (indice < 0) {
            printf("Error: no existe un alquiler con el numero %d.\n", numero);
            continue;
        }

        if (alquileres[indice].estado != ALQUILER_ACTIVO) {
            printf("Error: el alquiler %d esta %s; solo se pueden operar "
                   "alquileres activos.\n", numero,
                   textoEstadoAlquiler(alquileres[indice].estado));
            continue;
        }

        return indice;
    }

    return -1;
}

/* =========================================================
   OPERACIONES
   ========================================================= */

void realizarAlquiler(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler nuevo;
    Cliente cliente;
    Vehiculo vehiculo;
    Fecha inicio;
    char dni[12];
    char patente[12];
    int cantidad;
    int disponibles;
    int resultado;
    int dias;
    float importe;

    mostrarTitulo("REALIZAR ALQUILER");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    if (cantidad >= MAX_ALQUILERES) {
        printf("Error: se alcanzo el limite de %d alquileres.\n",
               MAX_ALQUILERES);
        return;
    }

    /* Se informa primero que vehiculos se pueden alquilar */
    disponibles = listarVehiculosDisponibles();

    if (disponibles <= 0) {
        return;
    }

    printf("\n");

    /* Cliente: debe existir y estar activo */
    while (1) {
        if (!pedirDNI(dni)) {
            mostrarCancelado();
            return;
        }

        resultado = obtenerCliente(dni, &cliente);

        if (resultado < 0) {
            return;
        }

        if (resultado == 0) {
            printf("Error: el cliente no existe. Registrelo primero en el "
                   "menu de clientes.\n");
            continue;
        }

        if (cliente.estado != CLIENTE_ACTIVO) {
            printf("Error: el cliente esta dado de baja.\n");
            continue;
        }

        break;
    }

    /* Vehiculo: debe existir y estar disponible */
    while (1) {
        if (!pedirPatente(patente)) {
            mostrarCancelado();
            return;
        }

        resultado = obtenerVehiculo(patente, &vehiculo);

        if (resultado < 0) {
            return;
        }

        if (resultado == 0) {
            printf("Error: el vehiculo no existe.\n");
            continue;
        }

        if (vehiculo.estado != VEHICULO_DISPONIBLE) {
            printf("Error: el vehiculo no esta disponible (estado: %s).\n",
                   textoEstadoVehiculo(vehiculo.estado));
            continue;
        }

        break;
    }

    if (!pedirFecha("Fecha de inicio del alquiler", &inicio)) {
        mostrarCancelado();
        return;
    }

    if (!pedirEntero("Cantidad de dias", 1, MAX_DIAS_ALQUILER, &dias)) {
        mostrarCancelado();
        return;
    }

    memset(&nuevo, 0, sizeof(Alquiler));
    nuevo.numero = nuevoNumero(alquileres, cantidad);
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
    printf("Cliente:           %s (%s)\n", cliente.nombre, cliente.dni);
    printf("Vehiculo:          %s %s (%s)\n", vehiculo.marca,
           vehiculo.modelo, vehiculo.patente);
    printf("Fecha de inicio:   ");
    imprimirFecha(nuevo.fechaInicio);
    printf("\nDevolucion prevista: ");
    imprimirFecha(nuevo.fechaPrevista);
    printf("\nCantidad de dias:  %d\n", dias);
    printf("Precio por dia:    $%.2f\n", vehiculo.precioDia);
    printf("Importe total:     $%.2f\n\n", importe);

    if (!confirmar("Confirmar alquiler")) {
        mostrarCancelado();
        return;
    }

    alquileres[cantidad] = nuevo;

    if (!guardarAlquileres(alquileres, cantidad + 1)) {
        return;
    }

    if (!actualizarEstadoVehiculo(patente, VEHICULO_ALQUILADO)) {
        /* Se deshace el alquiler para no dejar datos inconsistentes */
        if (!guardarAlquileres(alquileres, cantidad)) {
            printf("ADVERTENCIA: no se pudo deshacer el alquiler. "
                   "Revise los datos.\n");
        }

        printf("Error: no se pudo actualizar el estado del vehiculo. "
               "El alquiler no fue registrado.\n");
        return;
    }

    registrarMovimientoDe(&nuevo, MOV_ALQUILER, importe, inicio);

    printf("\nAlquiler registrado correctamente. Numero de alquiler: %d\n",
           nuevo.numero);
}

void registrarDevolucion(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler anterior;
    Alquiler *alquiler;
    Fecha devolucion;
    int cantidad;
    int indice;
    int diasReales;
    int diasExtra;
    float precioDia;
    float recargo;
    float importeFinal;

    mostrarTitulo("REGISTRAR DEVOLUCION");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    indice = seleccionarAlquilerActivo(alquileres, cantidad,
                                       "Numero de alquiler a devolver");

    if (indice < 0) {
        return;
    }

    alquiler = &alquileres[indice];

    while (1) {
        if (!pedirFecha("Fecha real de devolucion", &devolucion)) {
            mostrarCancelado();
            return;
        }

        if (compararFechas(devolucion, alquiler->fechaInicio) < 0) {
            printf("Error: la fecha de devolucion no puede ser anterior a "
                   "la fecha de inicio (");
            imprimirFecha(alquiler->fechaInicio);
            printf(").\n");
            continue;
        }

        break;
    }

    /* Se cobran como minimo los dias contratados; los dias que exceden
       la cantidad contratada se facturan al mismo precio pactado. */
    diasReales = diasEntre(alquiler->fechaInicio, devolucion);

    if (diasReales < 1) {
        diasReales = 1;
    }

    diasExtra = diasReales > alquiler->cantidadDias
                ? diasReales - alquiler->cantidadDias : 0;
    precioDia = precioDiarioPactado(alquiler);
    recargo = precioDia * (float)diasExtra;
    importeFinal = alquiler->importeTotal + recargo;

    printf("\n--- RESUMEN DE LA DEVOLUCION ---\n");
    printf("Alquiler Nro:       %d (%s - %s)\n", alquiler->numero,
           alquiler->dniCliente, alquiler->patente);
    printf("Dias contratados:   %d\n", alquiler->cantidadDias);
    printf("Dias transcurridos: %d\n", diasReales);
    printf("Dias adicionales:   %d\n", diasExtra);
    printf("Precio por dia:     $%.2f\n", precioDia);
    printf("Recargo:            $%.2f\n", recargo);
    printf("Importe final:      $%.2f\n", importeFinal);
    printf("Importe pagado:     $%.2f\n", alquiler->importePagado);
    printf("Saldo pendiente:    $%.2f\n\n",
           importeFinal - alquiler->importePagado > TOLERANCIA
           ? importeFinal - alquiler->importePagado : 0.0f);

    if (!confirmar("Confirmar devolucion")) {
        mostrarCancelado();
        return;
    }

    anterior = *alquiler;

    alquiler->fechaDevolucion = devolucion;
    alquiler->cantidadDias = alquiler->cantidadDias + diasExtra;
    alquiler->importeTotal = importeFinal;
    alquiler->estado = ALQUILER_FINALIZADO;

    if (!guardarAlquileres(alquileres, cantidad)) {
        return;
    }

    if (!actualizarEstadoVehiculo(alquiler->patente, VEHICULO_DISPONIBLE)) {
        *alquiler = anterior;

        if (!guardarAlquileres(alquileres, cantidad)) {
            printf("ADVERTENCIA: no se pudo deshacer la devolucion. "
                   "Revise los datos.\n");
        }

        printf("Error: no se pudo actualizar el vehiculo. "
               "La devolucion no fue registrada.\n");
        return;
    }

    if (diasExtra > 0) {
        registrarMovimientoDe(alquiler, MOV_AJUSTE, recargo, devolucion);
    }

    registrarMovimientoDe(alquiler, MOV_DEVOLUCION, importeFinal, devolucion);

    printf("\nDevolucion registrada correctamente.\n");

    if (saldoAlquiler(alquiler) > 0) {
        printf("El cliente adeuda $%.2f. Puede registrar el pago desde "
               "el menu de pagos.\n", saldoAlquiler(alquiler));
    }
}

void modificarAlquiler(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler *alquiler;
    Fecha fecha;
    int cantidad;
    int indice;
    int nuevosDias;
    float precioDia;
    float nuevoImporte;
    float diferencia;

    mostrarTitulo("MODIFICAR ALQUILER");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    indice = seleccionarAlquilerActivo(alquileres, cantidad,
                                       "Numero de alquiler a modificar");

    if (indice < 0) {
        return;
    }

    alquiler = &alquileres[indice];
    precioDia = precioDiarioPactado(alquiler);

    printf("\nSe puede modificar la duracion del alquiler "
           "(actual: %d dias, $%.2f por dia).\n",
           alquiler->cantidadDias, precioDia);

    if (!pedirEntero("Nueva cantidad de dias", 1, MAX_DIAS_ALQUILER,
                     &nuevosDias)) {
        mostrarCancelado();
        return;
    }

    if (nuevosDias == alquiler->cantidadDias) {
        printf("La cantidad de dias es la misma. No hay cambios.\n");
        return;
    }

    nuevoImporte = precioDia * (float)nuevosDias;

    if (nuevoImporte + TOLERANCIA < alquiler->importePagado) {
        printf("Error: el nuevo importe ($%.2f) es menor a lo ya pagado "
               "($%.2f). Elija mas dias o cancele el alquiler.\n",
               nuevoImporte, alquiler->importePagado);
        return;
    }

    diferencia = nuevoImporte - alquiler->importeTotal;

    printf("\nNuevo importe total: $%.2f (diferencia: %+.2f)\n",
           nuevoImporte, diferencia);

    if (!pedirFechaOperacion("Fecha de la modificacion", &fecha)) {
        mostrarCancelado();
        return;
    }

    if (!confirmar("Confirmar modificacion")) {
        mostrarCancelado();
        return;
    }

    alquiler->cantidadDias = nuevosDias;
    alquiler->fechaPrevista = sumarDias(alquiler->fechaInicio, nuevosDias);
    alquiler->importeTotal = nuevoImporte;

    if (!guardarAlquileres(alquileres, cantidad)) {
        return;
    }

    registrarMovimientoDe(alquiler, MOV_AJUSTE, diferencia, fecha);

    printf("\nAlquiler modificado correctamente. Nueva devolucion "
           "prevista: ");
    imprimirFecha(alquiler->fechaPrevista);
    printf("\n");
}

void cancelarAlquiler(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    Alquiler anterior;
    Alquiler *alquiler;
    Fecha fecha;
    int cantidad;
    int indice;
    float reembolso;

    mostrarTitulo("CANCELAR ALQUILER");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    indice = seleccionarAlquilerActivo(alquileres, cantidad,
                                       "Numero de alquiler a cancelar");

    if (indice < 0) {
        return;
    }

    alquiler = &alquileres[indice];
    reembolso = alquiler->importePagado;

    if (reembolso > TOLERANCIA) {
        printf("\nEl cliente habia pagado $%.2f, que se registrara como "
               "reembolso.\n", reembolso);
    }

    if (!pedirFechaOperacion("Fecha de la cancelacion", &fecha)) {
        mostrarCancelado();
        return;
    }

    if (!confirmar("Confirmar la cancelacion del alquiler")) {
        mostrarCancelado();
        return;
    }

    anterior = *alquiler;

    alquiler->estado = ALQUILER_CANCELADO;
    alquiler->importePagado = 0.0f;

    if (!guardarAlquileres(alquileres, cantidad)) {
        return;
    }

    if (!actualizarEstadoVehiculo(alquiler->patente, VEHICULO_DISPONIBLE)) {
        *alquiler = anterior;

        if (!guardarAlquileres(alquileres, cantidad)) {
            printf("ADVERTENCIA: no se pudo deshacer la cancelacion. "
                   "Revise los datos.\n");
        }

        printf("Error: no se pudo actualizar el vehiculo. "
               "El alquiler no fue cancelado.\n");
        return;
    }

    if (reembolso > TOLERANCIA) {
        registrarMovimientoDe(alquiler, MOV_AJUSTE, -reembolso, fecha);
    }

    printf("\nAlquiler cancelado correctamente. El vehiculo %s vuelve a "
           "estar disponible.\n", alquiler->patente);
}

void listarAlquileres(void)
{
    Alquiler alquileres[MAX_ALQUILERES];
    int cantidad;
    int i;

    mostrarTitulo("LISTADO DE ALQUILERES");

    if (!cargarAlquileres(alquileres, &cantidad)) {
        return;
    }

    if (cantidad == 0) {
        printf("No hay alquileres registrados.\n");
        return;
    }

    imprimirEncabezadoAlquileres();

    for (i = 0; i < cantidad; i++) {
        imprimirFilaAlquiler(&alquileres[i]);
    }

    printf("\nTotal de alquileres: %d\n", cantidad);
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
