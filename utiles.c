#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "utiles.h"

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

/* ---------- Lectura por teclado ---------- */

/* Lee una linea. Devuelve 0 si ya no hay mas datos para leer */
int leerLinea(const char *mensaje, char texto[], int tam)
{
    int leido = 0;
    int largo;
    int c;

    printf("%s", mensaje);

    if (fgets(texto, tam, stdin) != NULL) {
        leido = 1;
        largo = strlen(texto);

        if (largo > 0 && texto[largo - 1] == '\n') {
            texto[largo - 1] = '\0';
        } else {
            /* la linea era mas larga que el texto: se descarta el resto */
            while ((c = getchar()) != '\n' && c != EOF) {
            }
        }
    } else {
        texto[0] = '\0';
    }

    return leido;
}

/* Devuelve la opcion elegida, -1 si no es un numero y 0 si no hay mas datos */
int leerOpcion(const char *mensaje)
{
    char texto[100];
    char resto;
    int numero;
    int opcion = -1;

    if (!leerLinea(mensaje, texto, 100)) {
        opcion = 0;
    } else if (sscanf(texto, "%d%c", &numero, &resto) == 1 && numero >= 0) {
        opcion = numero;
    }

    return opcion;
}

int confirmar(const char *mensaje)
{
    char texto[100];
    char mensajeCompleto[150];
    int respuesta = -1;

    sprintf(mensajeCompleto, "%s (S/N): ", mensaje);

    while (respuesta == -1 && leerLinea(mensajeCompleto, texto, 100)) {
        if (strlen(texto) == 1 && (texto[0] == 's' || texto[0] == 'S')) {
            respuesta = 1;
        } else if (strlen(texto) == 1 && (texto[0] == 'n' || texto[0] == 'N')) {
            respuesta = 0;
        } else {
            printf("Error: responda S o N.\n");
        }
    }

    return respuesta == 1;
}

int pedirTexto(const char *mensaje, char texto[], int tam)
{
    char mensajeCompleto[150];
    int valido = 0;
    int cancelado = 0;

    sprintf(mensajeCompleto, "%s (0 para cancelar): ", mensaje);

    while (!valido && !cancelado && leerLinea(mensajeCompleto, texto, tam)) {
        if (strcmp(texto, "0") == 0) {
            cancelado = 1;
        } else if (strlen(texto) == 0) {
            printf("Error: no puede quedar vacio.\n");
        } else {
            valido = 1;
        }
    }

    return valido;
}

int pedirEntero(const char *mensaje, int min, int max, int *valor)
{
    char texto[100];
    char mensajeCompleto[150];
    char resto;
    int numero;
    int valido = 0;
    int cancelado = 0;

    sprintf(mensajeCompleto, "%s (%d a %d, 0 para cancelar): ", mensaje, min, max);

    while (!valido && !cancelado && leerLinea(mensajeCompleto, texto, 100)) {
        if (sscanf(texto, "%d%c", &numero, &resto) != 1) {
            printf("Error: debe ingresar un numero entero.\n");
        } else if (numero == 0) {
            cancelado = 1;
        } else if (numero < min || numero > max) {
            printf("Error: el numero debe estar entre %d y %d.\n", min, max);
        } else {
            *valor = numero;
            valido = 1;
        }
    }

    return valido;
}

/* Pide un importe mayor a cero. Si maximo es mayor a 0, el importe no puede superarlo */
int pedirMonto(const char *mensaje, float maximo, float *valor)
{
    char texto[100];
    char mensajeCompleto[150];
    char resto;
    float numero;
    int i;
    int valido = 0;
    int cancelado = 0;

    sprintf(mensajeCompleto, "%s (0 para cancelar): $", mensaje);

    while (!valido && !cancelado && leerLinea(mensajeCompleto, texto, 100)) {
        /* se acepta la coma como separador decimal */
        for (i = 0; texto[i] != '\0'; i++) {
            if (texto[i] == ',') {
                texto[i] = '.';
            }
        }

        if (sscanf(texto, "%f%c", &numero, &resto) != 1) {
            printf("Error: debe ingresar un importe numerico.\n");
        } else if (numero == 0) {
            cancelado = 1;
        } else if (numero < 0) {
            printf("Error: el importe debe ser mayor que cero.\n");
        } else if (maximo > 0 && numero > maximo + 0.005) {
            printf("Error: el importe no puede superar $%.2f\n", maximo);
        } else {
            *valor = numero;
            valido = 1;
        }
    }

    return valido;
}

int pedirDNI(char dni[])
{
    char texto[100];
    int largo;
    int i;
    int soloNumeros;
    int valido = 0;

    while (!valido && pedirTexto("DNI (7 u 8 numeros)", texto, 100)) {
        largo = strlen(texto);
        soloNumeros = 1;

        for (i = 0; i < largo; i++) {
            if (!isdigit((unsigned char)texto[i])) {
                soloNumeros = 0;
            }
        }

        if (soloNumeros && (largo == 7 || largo == 8)) {
            strcpy(dni, texto);
            valido = 1;
        } else {
            printf("Error: el DNI debe tener 7 u 8 numeros.\n");
        }
    }

    return valido;
}

/* Formato viejo: ABC123. Formato nuevo: AB123CD */
int patenteValida(const char *patente)
{
    int largo = strlen(patente);
    int i;
    int valida = 0;

    if (largo == 6) {
        valida = 1;

        for (i = 0; i < 3; i++) {
            if (!isalpha((unsigned char)patente[i])) {
                valida = 0;
            }
        }

        for (i = 3; i < 6; i++) {
            if (!isdigit((unsigned char)patente[i])) {
                valida = 0;
            }
        }
    } else if (largo == 7) {
        valida = 1;

        for (i = 0; i < 2; i++) {
            if (!isalpha((unsigned char)patente[i])) {
                valida = 0;
            }
        }

        for (i = 2; i < 5; i++) {
            if (!isdigit((unsigned char)patente[i])) {
                valida = 0;
            }
        }

        for (i = 5; i < 7; i++) {
            if (!isalpha((unsigned char)patente[i])) {
                valida = 0;
            }
        }
    }

    return valida;
}

int pedirPatente(char patente[])
{
    char texto[100];
    int i;
    int valido = 0;

    while (!valido && pedirTexto("Patente (ABC123 o AB123CD)", texto, 100)) {
        for (i = 0; texto[i] != '\0'; i++) {
            texto[i] = toupper((unsigned char)texto[i]);
        }

        if (patenteValida(texto)) {
            strcpy(patente, texto);
            valido = 1;
        } else {
            printf("Error: patente invalida. Use ABC123 o AB123CD.\n");
        }
    }

    return valido;
}

/* ---------- Fechas ---------- */

Fecha fechaActual(void)
{
    Fecha fecha;
    time_t ahora = time(NULL);
    struct tm *hoy = localtime(&ahora);

    fecha.dia = hoy->tm_mday;
    fecha.mes = hoy->tm_mon + 1;
    fecha.anio = hoy->tm_year + 1900;

    return fecha;
}

int esBisiesto(int anio)
{
    return (anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0;
}

int diasDelMes(int mes, int anio)
{
    int dias = 31;

    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        dias = 30;
    } else if (mes == 2) {
        dias = esBisiesto(anio) ? 29 : 28;
    }

    return dias;
}

int fechaValida(Fecha fecha)
{
    int valida = 0;

    if (fecha.anio >= 1900 && fecha.anio <= 2100 && fecha.mes >= 1 &&
        fecha.mes <= 12 && fecha.dia >= 1 &&
        fecha.dia <= diasDelMes(fecha.mes, fecha.anio)) {
        valida = 1;
    }

    return valida;
}

/* Cantidad de dias desde el 01/01/1900. Sirve para comparar y restar fechas */
long fechaADias(Fecha fecha)
{
    long dias = 0;
    int anio;
    int mes;

    for (anio = 1900; anio < fecha.anio; anio++) {
        dias += esBisiesto(anio) ? 366 : 365;
    }

    for (mes = 1; mes < fecha.mes; mes++) {
        dias += diasDelMes(mes, fecha.anio);
    }

    return dias + fecha.dia;
}

int compararFechas(Fecha a, Fecha b)
{
    long diasA = fechaADias(a);
    long diasB = fechaADias(b);
    int resultado = 0;

    if (diasA < diasB) {
        resultado = -1;
    } else if (diasA > diasB) {
        resultado = 1;
    }

    return resultado;
}

int diasEntre(Fecha desde, Fecha hasta)
{
    return (int)(fechaADias(hasta) - fechaADias(desde));
}

/* Suma dias de a uno (dias tiene que ser positivo) */
Fecha sumarDias(Fecha fecha, int dias)
{
    while (dias > 0) {
        fecha.dia++;

        if (fecha.dia > diasDelMes(fecha.mes, fecha.anio)) {
            fecha.dia = 1;
            fecha.mes++;

            if (fecha.mes > 12) {
                fecha.mes = 1;
                fecha.anio++;
            }
        }

        dias--;
    }

    return fecha;
}

void imprimirFecha(Fecha fecha)
{
    if (fecha.dia == 0) {
        printf("--/--/----");
    } else {
        printf("%02d/%02d/%04d", fecha.dia, fecha.mes, fecha.anio);
    }
}

/* Pide una fecha (solo para las consultas por periodo) */
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
        } else if (!pedirEntero("  Anio", 1900, 2100, &leida.anio)) {
            cancelado = 1;
        } else if (fechaValida(leida)) {
            *fecha = leida;
            valida = 1;
        } else {
            printf("Error: esa fecha no existe.\n");
        }
    }

    return valida;
}

/* ---------- Textos ---------- */

const char *textoEstadoCliente(EstadoCliente estado)
{
    const char *texto = "Inactivo";

    if (estado == CLIENTE_ACTIVO) {
        texto = "Activo";
    }

    return texto;
}

const char *textoEstadoVehiculo(EstadoVehiculo estado)
{
    const char *texto = "Disponible";

    if (estado == VEHICULO_ALQUILADO) {
        texto = "Alquilado";
    } else if (estado == VEHICULO_MANTENIMIENTO) {
        texto = "En mantenimiento";
    } else if (estado == VEHICULO_BAJA) {
        texto = "Baja";
    }

    return texto;
}

const char *textoEstadoAlquiler(EstadoAlquiler estado)
{
    const char *texto = "Activo";

    if (estado == ALQUILER_FINALIZADO) {
        texto = "Finalizado";
    } else if (estado == ALQUILER_CANCELADO) {
        texto = "Cancelado";
    }

    return texto;
}

const char *textoTipoMovimiento(TipoMovimiento tipo)
{
    const char *texto = "Alquiler";

    if (tipo == MOV_PAGO) {
        texto = "Pago";
    } else if (tipo == MOV_DEVOLUCION) {
        texto = "Devolucion";
    } else if (tipo == MOV_AJUSTE) {
        texto = "Ajuste";
    }

    return texto;
}
