#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "utiles.h"

#define TAM_LINEA 256

/* =========================================================
   PRESENTACION
   ========================================================= */

void mostrarTitulo(const char *titulo)
{
    printf("\n========================================\n");
    printf("  %s\n", titulo);
    printf("========================================\n");
}

void mostrarCancelado(void)
{
    printf("Operacion cancelada. No se guardaron cambios.\n");
}

/* =========================================================
   ENTRADA POR TECLADO
   ========================================================= */

/* Elimina espacios al inicio y al final */
static void recortar(char texto[])
{
    size_t inicio = 0;
    size_t fin = strlen(texto);

    while (fin > 0 && isspace((unsigned char)texto[fin - 1])) {
        fin--;
    }

    texto[fin] = '\0';

    while (texto[inicio] != '\0' &&
           isspace((unsigned char)texto[inicio])) {
        inicio++;
    }

    if (inicio > 0) {
        memmove(texto, texto + inicio, strlen(texto + inicio) + 1);
    }
}

/* Lee una linea completa. Devuelve 0 si se llego al fin de la entrada. */
int leerLinea(const char *mensaje, char texto[], int tam)
{
    size_t largo;
    int c;

    if (mensaje != NULL) {
        printf("%s", mensaje);
        fflush(stdout);
    }

    if (fgets(texto, tam, stdin) == NULL) {
        texto[0] = '\0';
        return 0;
    }

    largo = strlen(texto);

    if (largo > 0 && texto[largo - 1] == '\n') {
        texto[largo - 1] = '\0';
    } else {
        /* Linea mas larga que el buffer: se descarta el resto */
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    recortar(texto);
    return 1;
}

/* Convierte un texto a entero verificando que sea todo numerico */
static int parsearEntero(const char *texto, long *valor)
{
    char *fin;
    long numero;

    if (texto[0] == '\0') {
        return 0;
    }

    errno = 0;
    numero = strtol(texto, &fin, 10);

    if (errno != 0 || *fin != '\0') {
        return 0;
    }

    *valor = numero;
    return 1;
}

int leerOpcion(const char *mensaje)
{
    char texto[TAM_LINEA];
    long numero;

    if (!leerLinea(mensaje, texto, sizeof(texto))) {
        return 0;
    }

    if (!parsearEntero(texto, &numero) || numero < 0 || numero > 1000) {
        return -1;
    }

    return (int)numero;
}

int confirmar(const char *mensaje)
{
    char prompt[200];
    char texto[TAM_LINEA];
    char primera;

    snprintf(prompt, sizeof(prompt), "%s (S/N): ", mensaje);

    while (leerLinea(prompt, texto, sizeof(texto))) {
        primera = (char)toupper((unsigned char)texto[0]);

        if (texto[0] != '\0' && (texto[1] == '\0' ||
            (texto[2] == '\0' && (primera == 'S' || primera == 'N')))) {
            if (primera == 'S') {
                return 1;
            }

            if (primera == 'N') {
                return 0;
            }
        }

        printf("Error: responda S (si) o N (no).\n");
    }

    return 0;
}

int pedirTexto(const char *mensaje, char texto[], int tam)
{
    char prompt[200];

    snprintf(prompt, sizeof(prompt), "%s (0 para cancelar): ", mensaje);

    while (leerLinea(prompt, texto, tam)) {
        if (strcmp(texto, "0") == 0) {
            return 0;
        }

        if (texto[0] == '\0') {
            printf("Error: el campo no puede quedar vacio.\n");
            continue;
        }

        return 1;
    }

    return 0;
}

int pedirCampo(const char *mensaje, char destino[], int tam,
               int (*validar)(const char *), const char *mensajeError)
{
    char temporal[TAM_LINEA];

    while (pedirTexto(mensaje, temporal, sizeof(temporal))) {
        if ((int)strlen(temporal) >= tam) {
            printf("Error: el texto no puede superar los %d caracteres.\n",
                   tam - 1);
            continue;
        }

        if (validar != NULL && !validar(temporal)) {
            printf("Error: %s\n", mensajeError);
            continue;
        }

        strcpy(destino, temporal);
        return 1;
    }

    return 0;
}

int pedirEntero(const char *mensaje, int min, int max, int *valor)
{
    char prompt[200];
    char texto[TAM_LINEA];
    long numero;

    snprintf(prompt, sizeof(prompt), "%s (%d-%d, 0 para cancelar): ",
             mensaje, min, max);

    while (leerLinea(prompt, texto, sizeof(texto))) {
        if (!parsearEntero(texto, &numero)) {
            printf("Error: debe ingresar un numero entero.\n");
            continue;
        }

        if (numero == 0) {
            return 0;
        }

        if (numero < min || numero > max) {
            printf("Error: el valor debe estar entre %d y %d.\n", min, max);
            continue;
        }

        *valor = (int)numero;
        return 1;
    }

    return 0;
}

int pedirMonto(const char *mensaje, float maximo, float *valor)
{
    char prompt[200];
    char texto[TAM_LINEA];
    char *fin;
    double numero;
    int i;

    snprintf(prompt, sizeof(prompt), "%s (0 para cancelar): $", mensaje);

    while (leerLinea(prompt, texto, sizeof(texto))) {
        /* Se acepta la coma como separador decimal */
        for (i = 0; texto[i] != '\0'; i++) {
            if (texto[i] == ',') {
                texto[i] = '.';
            }
        }

        if (texto[0] == '\0') {
            printf("Error: debe ingresar un importe.\n");
            continue;
        }

        errno = 0;
        numero = strtod(texto, &fin);

        if (*fin != '\0' || errno != 0 || numero != numero) {
            printf("Error: el importe debe ser un numero.\n");
            continue;
        }

        if (numero == 0) {
            return 0;
        }

        if (numero < 0) {
            printf("Error: el importe debe ser mayor que cero.\n");
            continue;
        }

        if (numero > 100000000.0) {
            printf("Error: el importe ingresado es demasiado grande.\n");
            continue;
        }

        if (maximo > 0 && numero > (double)maximo + 0.005) {
            printf("Error: el importe no puede superar $%.2f.\n", maximo);
            continue;
        }

        *valor = (float)numero;

        if (maximo > 0 && *valor > maximo) {
            *valor = maximo;
        }

        return 1;
    }

    return 0;
}

int pedirDNI(char dni[])
{
    char texto[TAM_LINEA];
    int largo;
    int i;
    int valido;

    while (pedirTexto("DNI (7 u 8 digitos)", texto, sizeof(texto))) {
        largo = (int)strlen(texto);
        valido = (largo == 7 || largo == 8);

        for (i = 0; valido && texto[i] != '\0'; i++) {
            if (!isdigit((unsigned char)texto[i])) {
                valido = 0;
            }
        }

        if (valido) {
            strcpy(dni, texto);
            return 1;
        }

        printf("Error: el DNI debe tener 7 u 8 digitos numericos.\n");
    }

    return 0;
}

/* ---------- Patentes ---------- */

/* L = letra, N = digito */
static int coincidePatron(const char *texto, const char *patron)
{
    size_t i;

    if (strlen(texto) != strlen(patron)) {
        return 0;
    }

    for (i = 0; patron[i] != '\0'; i++) {
        if (patron[i] == 'L' && !isalpha((unsigned char)texto[i])) {
            return 0;
        }

        if (patron[i] == 'N' && !isdigit((unsigned char)texto[i])) {
            return 0;
        }
    }

    return 1;
}

/* Formato anterior (ABC123) o Mercosur (AB123CD) */
int patenteValida(const char *patente)
{
    return coincidePatron(patente, "LLLNNN") ||
           coincidePatron(patente, "LLNNNLL");
}

static void aMayusculas(char texto[])
{
    int i;

    for (i = 0; texto[i] != '\0'; i++) {
        texto[i] = (char)toupper((unsigned char)texto[i]);
    }
}

int pedirPatente(char patente[])
{
    char texto[TAM_LINEA];

    while (pedirTexto("Patente (ABC123 o AB123CD)", texto, sizeof(texto))) {
        aMayusculas(texto);

        if (patenteValida(texto)) {
            strcpy(patente, texto);
            return 1;
        }

        printf("Error: formato de patente invalido. "
               "Use ABC123 o AB123CD.\n");
    }

    return 0;
}

/* =========================================================
   FECHAS
   ========================================================= */

Fecha fechaActual(void)
{
    Fecha fecha;
    time_t ahora = time(NULL);
    struct tm *actual = localtime(&ahora);

    if (actual != NULL) {
        fecha.dia = actual->tm_mday;
        fecha.mes = actual->tm_mon + 1;
        fecha.anio = actual->tm_year + 1900;
    } else {
        fecha.dia = 1;
        fecha.mes = 1;
        fecha.anio = ANIO_MINIMO;
    }

    return fecha;
}

int fechaValida(Fecha fecha)
{
    int diasMes;
    int bisiesto;

    if (fecha.anio < ANIO_MINIMO || fecha.anio > ANIO_MAXIMO ||
        fecha.mes < 1 || fecha.mes > 12 || fecha.dia < 1) {
        return 0;
    }

    bisiesto = (fecha.anio % 400 == 0) ||
               (fecha.anio % 4 == 0 && fecha.anio % 100 != 0);

    if (fecha.mes == 2) {
        diasMes = bisiesto ? 29 : 28;
    } else if (fecha.mes == 4 || fecha.mes == 6 ||
               fecha.mes == 9 || fecha.mes == 11) {
        diasMes = 30;
    } else {
        diasMes = 31;
    }

    return fecha.dia <= diasMes;
}

/* Cantidad de dias desde el 01/01/1970 (calendario gregoriano) */
long fechaADias(Fecha fecha)
{
    long anio = fecha.anio;
    long mes = fecha.mes;
    long era, anioEra, diaAnio, diaEra;

    if (mes <= 2) {
        anio--;
    }

    era = (anio >= 0 ? anio : anio - 399) / 400;
    anioEra = anio - era * 400;
    diaAnio = (153 * (mes + (mes > 2 ? -3 : 9)) + 2) / 5 + fecha.dia - 1;
    diaEra = anioEra * 365 + anioEra / 4 - anioEra / 100 + diaAnio;

    return era * 146097 + diaEra - 719468;
}

Fecha diasAFecha(long dias)
{
    Fecha fecha;
    long era, diaEra, anioEra, anio, diaAnio, mp;

    dias += 719468;
    era = (dias >= 0 ? dias : dias - 146096) / 146097;
    diaEra = dias - era * 146097;
    anioEra = (diaEra - diaEra / 1460 + diaEra / 36524 -
               diaEra / 146096) / 365;
    anio = anioEra + era * 400;
    diaAnio = diaEra - (365 * anioEra + anioEra / 4 - anioEra / 100);
    mp = (5 * diaAnio + 2) / 153;

    fecha.dia = (int)(diaAnio - (153 * mp + 2) / 5 + 1);
    fecha.mes = (int)(mp < 10 ? mp + 3 : mp - 9);
    fecha.anio = (int)(anio + (fecha.mes <= 2 ? 1 : 0));

    return fecha;
}

int compararFechas(Fecha a, Fecha b)
{
    long da = fechaADias(a);
    long db = fechaADias(b);

    if (da < db) {
        return -1;
    }

    return da > db ? 1 : 0;
}

int diasEntre(Fecha desde, Fecha hasta)
{
    return (int)(fechaADias(hasta) - fechaADias(desde));
}

Fecha sumarDias(Fecha fecha, int dias)
{
    return diasAFecha(fechaADias(fecha) + dias);
}

void imprimirFecha(Fecha fecha)
{
    if (fecha.dia == 0) {
        printf("--/--/----");
    } else {
        printf("%02d/%02d/%04d", fecha.dia, fecha.mes, fecha.anio);
    }
}

int pedirFecha(const char *titulo, Fecha *fecha)
{
    Fecha leida;

    printf("%s\n", titulo);

    while (1) {
        if (!pedirEntero("  Dia", 1, 31, &leida.dia)) {
            return 0;
        }

        if (!pedirEntero("  Mes", 1, 12, &leida.mes)) {
            return 0;
        }

        if (!pedirEntero("  Anio", ANIO_MINIMO, ANIO_MAXIMO, &leida.anio)) {
            return 0;
        }

        if (fechaValida(leida)) {
            *fecha = leida;
            return 1;
        }

        printf("Error: la fecha %02d/%02d/%04d no existe en el calendario.\n",
               leida.dia, leida.mes, leida.anio);
    }
}

/* Ofrece usar la fecha del sistema o ingresar otra */
int pedirFechaOperacion(const char *titulo, Fecha *fecha)
{
    Fecha hoy = fechaActual();

    printf("%s - fecha actual: ", titulo);
    imprimirFecha(hoy);
    printf("\n");

    if (confirmar("Usar la fecha actual")) {
        *fecha = hoy;
        return 1;
    }

    return pedirFecha("Ingrese la fecha de la operacion", fecha);
}

/* =========================================================
   TEXTOS DE ESTADOS
   ========================================================= */

const char *textoEstadoCliente(EstadoCliente estado)
{
    return estado == CLIENTE_ACTIVO ? "Activo" : "Inactivo";
}

const char *textoEstadoVehiculo(EstadoVehiculo estado)
{
    switch (estado) {
        case VEHICULO_DISPONIBLE:    return "Disponible";
        case VEHICULO_ALQUILADO:     return "Alquilado";
        case VEHICULO_MANTENIMIENTO: return "En mantenimiento";
        case VEHICULO_BAJA:          return "Baja";
        default:                     return "Desconocido";
    }
}

const char *textoEstadoAlquiler(EstadoAlquiler estado)
{
    switch (estado) {
        case ALQUILER_ACTIVO:     return "Activo";
        case ALQUILER_FINALIZADO: return "Finalizado";
        case ALQUILER_CANCELADO:  return "Cancelado";
        default:                  return "Desconocido";
    }
}

const char *textoTipoMovimiento(TipoMovimiento tipo)
{
    switch (tipo) {
        case MOV_ALQUILER:   return "Alquiler";
        case MOV_PAGO:       return "Pago";
        case MOV_DEVOLUCION: return "Devolucion";
        case MOV_AJUSTE:     return "Ajuste";
        default:             return "Desconocido";
    }
}
