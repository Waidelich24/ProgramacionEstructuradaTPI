#ifndef ERRORES_H
#define ERRORES_H

/* Codigos de error compartidos por todos los modulos.
   Cada funcion guarda su resultado en una variable 'error' y la devuelve
   en su unico return. */
typedef enum {
    ERR_OK = 0,
    ERR_CANCELADO,        /* el usuario cancelo la operacion */
    ERR_ARCHIVO_ABRIR,    /* el archivo no pudo abrirse */
    ERR_ARCHIVO_LEER,     /* fallo la lectura del archivo */
    ERR_ARCHIVO_ESCRIBIR, /* fallo la escritura del archivo */
    ERR_NO_ENCONTRADO,    /* el registro buscado no existe */
    ERR_DUPLICADO,        /* el registro ya existe */
    ERR_DATO_INVALIDO,    /* dato ingresado incorrecto */
    ERR_ESTADO_INVALIDO,  /* operacion no permitida por el estado actual */
    ERR_SIN_DATOS,        /* no hay registros para operar */
    ERR_NO_REGISTRADO     /* no se pudo registrar el movimiento */
} CodigoError;

#endif
