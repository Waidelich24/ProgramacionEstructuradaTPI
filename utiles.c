#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "utiles.h"

#define TAM_LINEA 256

/* =========================================================
   PRESENTACION Y ERRORES
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

const char *textoError(CodigoError error)
{
    const char *texto;

    switch (error) {
        case ERR_OK:               texto = "sin errores";                    break;
        case ERR_CANCELADO:        texto = "operacion cancelada";            break;
        case ERR_ARCHIVO_ABRIR:    texto = "no se pudo abrir el archivo";    break;
        case ERR_ARCHIVO_LEER:     texto = "no se pudo leer el archivo";     break;
        case ERR_ARCHIVO_ESCRIBIR: texto = "no se pudo escribir el archivo"; break;
        case ERR_NO_ENCONTRADO:    texto = "el registro no existe";          break;
        case ERR_DUPLICADO:        texto = "el registro ya existe";          break;
        case ERR_DATO_INVALIDO:    texto = "dato invalido";                  break;
        case ERR_ESTADO_INVALIDO:  texto = "operacion no permitida por el estado actual"; break;
        case ERR_SIN_DATOS:        texto = "no hay registros para operar";   break;
        case ERR_NO_REGISTRADO:    texto = "no se pudo registrar el movimiento"; break;
        default:                   texto = "error desconocido";              break;
    }

    return texto;
}

void mostrarError(CodigoError error, const char *detalle)
{
    printf("Error: %s", textoError(error));

    if (detalle != NULL) {
        printf(" (%s)", detalle);
    }

    printf(".\n");
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
    int leido = 0;

    if (mensaje != NULL) {
        printf("%s", mensaje);
        fflush(stdout);
    }

    if (fgets(texto, tam, stdin) == NULL) {
        texto[0] = '\0';
    } else {
        leido = 1;
        largo = strlen(texto);

        if (largo > 0 && texto[largo - 1] == '\n') {
            texto[largo - 1] = '\0';
        } else {
            /* Linea mas larga que el buffer: se descarta el resto */
            while ((c = getchar()) != '\n' && c != EOF) {
            }
        }

        recortar(texto);
    }

    return leido;
}

/* Convierte un texto a entero verificando que sea todo numerico */
static int parsearEntero(const char *texto, long *valor)
{
    char *fin;
    long numero;
    int valido = 0;

    if (texto[0] != '\0') {
        errno = 0;
        numero = strtol(texto, &fin, 10);

        if (errno == 0 && *fin == '\0') {
            *valor = numero;
            valido = 1;
        }
    }

    return valido;
}

int leerOpcion(const char *mensaje)
{
    char texto[TAM_LINEA];
    long numero;
    int opcion = -1;

    if (!leerLinea(mensaje, texto, sizeof(texto))) {
        opcion = 0;
    } else if (parsearEntero(texto, &numero) && numero >= 0 && numero <= 1000) {
        opcion = (int)numero;
    }

    return opcion;
}

int confirmar(const char *mensaje)
{
    char prompt[200];
    char texto[TAM_LINEA];
    size_t largo;
    int respuesta = -1;

    snprintf(prompt, sizeof(prompt), "%s (S/N): ", mensaje);

    while (respuesta < 0 && leerLinea(prompt, texto, sizeof(texto))) {
        largo = strlen(texto);

        if (largo == 1 && toupper((unsigned char)texto[0]) == 'S') {
            respuesta = 1;
        } else if (largo == 1 && toupper((unsigned char)texto[0]) == 'N') {
            respuesta = 0;
        } else if (largo == 2 && toupper((unsigned char)texto[0]) == 'S' &&
                   toupper((unsigned char)texto[1]) == 'I') {
            respuesta = 1;
        } else if (largo == 2 && toupper((unsigned char)texto[0]) == 'N' &&
                   toupper((unsigned char)texto[1]) == 'O') {
            respuesta = 0;
        } else {
            printf("Error: responda S (si) o N (no).\n");
        }
    }

    return respuesta == 1;
}

int pedirTexto(const char *mensaje, char texto[], int tam)
{
    char prompt[200];
    int valido = 0;
    int terminado = 0;

    snprintf(prompt, sizeof(prompt), "%s (0 para cancelar): ", mensaje);

    while (!terminado && leerLinea(prompt, texto, tam)) {
        if (strcmp(texto, "0") == 0) {
            terminado = 1;
        } else if (texto[0] == '\0') {
            printf("Error: el campo no puede quedar vacio.\n");
        } else {
            valido = 1;
            terminado = 1;
        }
    }

    return valido;
}

int pedirCampo(const char *mensaje, char destino[], int tam,
               int (*validar)(const char *), const char *mensajeError)
{
    char temporal[TAM_LINEA];
    int valido = 0;

    while (!valido && pedirTexto(mensaje, temporal, sizeof(temporal))) {
        if ((int)strlen(temporal) >= tam) {
            printf("Error: el texto no puede superar los %d caracteres.\n",
                   tam - 1);
        } else if (validar != NULL && !validar(temporal)) {
            printf("Error: %s\n", mensajeError);
        } else {
            strcpy(destino, temporal);
            valido = 1;
        }
    }

    return valido;
}

int pedirEntero(const char *mensaje, int min, int max, int *valor)
{
    char prompt[200];
    char texto[TAM_LINEA];
    long numero;
    int valido = 0;
    int terminado = 0;

    snprintf(prompt, sizeof(prompt), "%s (%d-%d, 0 para cancelar): ",
             mensaje, min, max);

    while (!terminado && leerLinea(prompt, texto, sizeof(texto))) {
        if (!parsearEntero(texto, &numero)) {
            printf("Error: debe ingresar un numero entero.\n");
        } else if (numero == 0) {
            terminado = 1;
        } else if (numero < min || numero > max) {
            printf("Error: el valor debe estar entre %d y %d.\n", min, max);
        } else {
            *valor = (int)numero;
            valido = 1;
            terminado = 1;
        }
    }

    return valido;
}

int pedirMonto(const char *mensaje, float maximo, float *valor)
{
    char prompt[200];
    char texto[TAM_LINEA];
    char *fin;
    double numero;
    int i;
    int valido = 0;
    int terminado = 0;

    snprintf(prompt, sizeof(prompt), "%s (0 para cancelar): $", mensaje);

    while (!terminado && leerLinea(prompt, texto, sizeof(texto))) {
        /* Se acepta la coma como separador decimal */
        for (i = 0; texto[i] != '\0'; i++) {
            if (texto[i] == ',') {
                texto[i] = '.';
            }
        }

        errno = 0;
        numero = strtod(texto, &fin);

        if (texto[0] == '\0') {
            printf("Error: debe ingresar un importe.\n");
        } else if (*fin != '\0' || errno != 0 || numero != numero) {
            printf("Error: el importe debe ser un numero.\n");
        } else if (numero == 0) {
            terminado = 1;
        } else if (numero < 0) {
            printf("Error: el importe debe ser mayor que cero.\n");
        } else if (numero > 100000000.0) {
            printf("Error: el importe ingresado es demasiado grande.\n");
        } else if (maximo > 0 && numero > (double)maximo + 0.005) {
            printf("Error: el importe no puede superar $%.2f.\n", maximo);
        } else {
            *valor = (float)numero;

            if (maximo > 0 && *valor > maximo) {
                *valor = maximo;
            }

            valido = 1;
            terminado = 1;
        }
    }

    return valido;
}

int pedirDNI(char dni[])
{
    char texto[TAM_LINEA];
    int largo;
    int i;
    int digitos;
    int valido = 0;

    while (!valido && pedirTexto("DNI (7 u 8 digitos)", texto, sizeof(texto))) {
        largo = (int)strlen(texto);
        digitos = 1;

        for (i = 0; texto[i] != '\0'; i++) {
            if (!isdigit((unsigned char)texto[i])) {
                digitos = 0;
            }
        }

        if (digitos && (largo == 7 || largo == 8)) {
            strcpy(dni, texto);
            valido = 1;
        } else {
            printf("Error: el DNI debe tener 7 u 8 digitos numericos.\n");
        }
    }

    return valido;
}

/* ---------- Patentes ---------- */

/* L = letra, N = digito */
static int coincidePatron(const char *texto, const char *patron)
{
    size_t i;
    int coincide = (strlen(texto) == strlen(patron));

    for (i = 0; coincide && patron[i] != '\0'; i++) {
        if (patron[i] == 'L' && !isalpha((unsigned char)texto[i])) {
            coincide = 0;
        }

        if (patron[i] == 'N' && !isdigit((unsigned char)texto[i])) {
            coincide = 0;
        }
    }

    return coincide;
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
    int valido = 0;

    while (!valido &&
           pedirTexto("Patente (ABC123 o AB123CD)", texto, sizeof(texto))) {
        aMayusculas(texto);

        if (patenteValida(texto)) {
            strcpy(patente, texto);
            valido = 1;
        } else {
            printf("Error: formato de patente invalido. "
                   "Use ABC123 o AB123CD.\n");
        }
    }

    return valido;
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
    int valida = 0;

    if (fecha.anio >= ANIO_MINIMO && fecha.anio <= ANIO_MAXIMO &&
        fecha.mes >= 1 && fecha.mes <= 12 && fecha.dia >= 1) {
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

        valida = fecha.dia <= diasMes;
    }

    return valida;
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
    int resultado = 0;

    if (da < db) {
        resultado = -1;
    } else if (da > db) {
        resultado = 1;
    }

    return resultado;
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
    int valida = 0;
    int cancelado = 0;

    printf("%s\n", titulo);

    while (!valida && !cancelado) {
        if (!pedirEntero("  Dia", 1, 31, &leida.dia)) {
            cancelado = 1;
        } else if (!pedirEntero("  Mes", 1, 12, &leida.mes)) {
            cancelado = 1;
        } else if (!pedirEntero("  Anio", ANIO_MINIMO, ANIO_MAXIMO,
                                &leida.anio)) {
            cancelado = 1;
        } else if (fechaValida(leida)) {
            *fecha = leida;
            valida = 1;
        } else {
            printf("Error: la fecha %02d/%02d/%04d no existe en el "
                   "calendario.\n", leida.dia, leida.mes, leida.anio);
        }
    }

    return valida;
}

/* Ofrece usar la fecha del sistema o ingresar otra */
int pedirFechaOperacion(const char *titulo, Fecha *fecha)
{
    Fecha hoy = fechaActual();
    int valida;

    printf("%s - fecha actual: ", titulo);
    imprimirFecha(hoy);
    printf("\n");

    if (confirmar("Usar la fecha actual")) {
        *fecha = hoy;
        valida = 1;
    } else {
        valida = pedirFecha("Ingrese la fecha de la operacion", fecha);
    }

    return valida;
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
    const char *texto;

    switch (estado) {
        case VEHICULO_DISPONIBLE:    texto = "Disponible";       break;
        case VEHICULO_ALQUILADO:     texto = "Alquilado";        break;
        case VEHICULO_MANTENIMIENTO: texto = "En mantenimiento"; break;
        case VEHICULO_BAJA:          texto = "Baja";             break;
        default:                     texto = "Desconocido";      break;
    }

    return texto;
}

const char *textoEstadoAlquiler(EstadoAlquiler estado)
{
    const char *texto;

    switch (estado) {
        case ALQUILER_ACTIVO:     texto = "Activo";      break;
        case ALQUILER_FINALIZADO: texto = "Finalizado";  break;
        case ALQUILER_CANCELADO:  texto = "Cancelado";   break;
        default:                  texto = "Desconocido"; break;
    }

    return texto;
}

const char *textoTipoMovimiento(TipoMovimiento tipo)
{
    const char *texto;

    switch (tipo) {
        case MOV_ALQUILER:   texto = "Alquiler";    break;
        case MOV_PAGO:       texto = "Pago";        break;
        case MOV_DEVOLUCION: texto = "Devolucion";  break;
        case MOV_AJUSTE:     texto = "Ajuste";      break;
        default:             texto = "Desconocido"; break;
    }

    return texto;
}
