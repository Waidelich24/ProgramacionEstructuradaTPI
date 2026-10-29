#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/* Estados de los registros */

typedef enum {
    CLIENTE_INACTIVO = 0,
    CLIENTE_ACTIVO = 1
} EstadoCliente;

typedef enum {
    VEHICULO_DISPONIBLE = 0,
    VEHICULO_ALQUILADO = 1,
    VEHICULO_MANTENIMIENTO = 2,
    VEHICULO_BAJA = 3
} EstadoVehiculo;

typedef enum {
    ALQUILER_ACTIVO = 0,
    ALQUILER_FINALIZADO = 1,
    ALQUILER_CANCELADO = 2
} EstadoAlquiler;

typedef enum {
    MOV_ALQUILER = 0,
    MOV_PAGO = 1,
    MOV_DEVOLUCION = 2,
    MOV_AJUSTE = 3
} TipoMovimiento;

/* Fecha */

typedef struct {
    int dia;
    int mes;
    int anio;
} Fecha;

/* Cliente */

typedef struct {
    char dni[12];
    char nombre[80];
    char telefono[24];
    char email[80];
    char direccion[120];
    Fecha fechaAlta;
    EstadoCliente estado;
} Cliente;

/* Vehiculo */

typedef struct {
    char patente[12];
    char marca[40];
    char modelo[40];
    int anio;
    char tipo[30];
    int capacidad;
    float precioDia;
    EstadoVehiculo estado;
} Vehiculo;

/* Alquiler */

typedef struct {
    int numero;
    char dniCliente[12];
    char patente[12];

    Fecha fechaInicio;
    Fecha fechaPrevista;
    Fecha fechaDevolucion;

    int cantidadDias;
    float importeTotal;
    float importePagado;

    EstadoAlquiler estado;
} Alquiler;

/* Movimiento economico */

typedef struct {
    int numero;
    int numeroAlquiler;
    TipoMovimiento tipo;
    float monto;
    Fecha fecha;

    char dniCliente[12];
    char patente[12];
} Movimiento;

#endif
