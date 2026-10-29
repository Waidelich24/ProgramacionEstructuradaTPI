#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"

int main(void) {
    FILE *archivo;
    Cliente clientes[10];
    Vehiculo vehiculos[10];
    
    /* --- 1. CREAR CLIENTES (10 registros) --- */
    memset(clientes, 0, sizeof(clientes));

    // Cliente 1
    strcpy(clientes[0].dni, "38111222");
    strcpy(clientes[0].nombre, "Perez, Juan Carlos");
    strcpy(clientes[0].telefono, "3764112233");
    strcpy(clientes[0].email, "juan.perez@email.com");
    strcpy(clientes[0].direccion, "Av. Corrientes 1540");
    clientes[0].fechaAlta.dia = 10; clientes[0].fechaAlta.mes = 3; clientes[0].fechaAlta.anio = 2026;
    clientes[0].estado = CLIENTE_ACTIVO;

    // Cliente 2
    strcpy(clientes[1].dni, "40222333");
    strcpy(clientes[1].nombre, "Gomez, Maria Laura");
    strcpy(clientes[1].telefono, "3764223344");
    strcpy(clientes[1].email, "mlgomez@email.com");
    strcpy(clientes[1].direccion, "Calle San Martin 450");
    clientes[1].fechaAlta.dia = 12; clientes[1].fechaAlta.mes = 3; clientes[1].fechaAlta.anio = 2026;
    clientes[1].estado = CLIENTE_ACTIVO;

    // Cliente 3
    strcpy(clientes[2].dni, "35333444");
    strcpy(clientes[2].nombre, "Rodriquez, Esteban");
    strcpy(clientes[2].telefono, "3764334455");
    strcpy(clientes[2].email, "esteban_r@email.com");
    strcpy(clientes[2].direccion, "Av. Mitre 1820");
    clientes[2].fechaAlta.dia = 15; clientes[2].fechaAlta.mes = 3; clientes[2].fechaAlta.anio = 2026;
    clientes[2].estado = CLIENTE_ACTIVO;

    // Cliente 4
    strcpy(clientes[3].dni, "42444555");
    strcpy(clientes[3].nombre, "Benitez, Sofia");
    strcpy(clientes[3].telefono, "3764445566");
    strcpy(clientes[3].email, "sofi.benitez@email.com");
    strcpy(clientes[3].direccion, "Ayacucho 1200");
    clientes[3].fechaAlta.dia = 20; clientes[3].fechaAlta.mes = 3; clientes[3].fechaAlta.anio = 2026;
    clientes[3].estado = CLIENTE_ACTIVO;

    // Cliente 5
    strcpy(clientes[4].dni, "32555666");
    strcpy(clientes[4].nombre, "Silva, Carlos Alberto");
    strcpy(clientes[4].telefono, "3764556677");
    strcpy(clientes[4].email, "csilva@email.com");
    strcpy(clientes[4].direccion, "Félix de Azara 850");
    clientes[4].fechaAlta.dia = 25; clientes[4].fechaAlta.mes = 3; clientes[4].fechaAlta.anio = 2026;
    clientes[4].estado = CLIENTE_ACTIVO;

    // Cliente 6
    strcpy(clientes[5].dni, "39666777");
    strcpy(clientes[5].nombre, "Martínez, Romina");
    strcpy(clientes[5].telefono, "3764667788");
    strcpy(clientes[5].email, "romi_m@email.com");
    strcpy(clientes[5].direccion, "Córdoba 1930");
    clientes[5].fechaAlta.dia = 1; clientes[5].fechaAlta.mes = 4; clientes[5].fechaAlta.anio = 2026;
    clientes[5].estado = CLIENTE_ACTIVO;

    // Cliente 7
    strcpy(clientes[6].dni, "36777888");
    strcpy(clientes[6].nombre, "Fernández, Lucas");
    strcpy(clientes[6].telefono, "3764778899");
    strcpy(clientes[6].email, "l_fernandez@email.com");
    strcpy(clientes[6].direccion, "Bolívar 420");
    clientes[6].fechaAlta.dia = 5; clientes[6].fechaAlta.mes = 4; clientes[6].fechaAlta.anio = 2026;
    clientes[6].estado = CLIENTE_ACTIVO;

    // Cliente 8
    strcpy(clientes[7].dni, "41888999");
    strcpy(clientes[7].nombre, "Acosta, Yamila");
    strcpy(clientes[7].telefono, "3764889900");
    strcpy(clientes[7].email, "yami_acosta@email.com");
    strcpy(clientes[7].direccion, "Rivadavia 2100");
    clientes[7].fechaAlta.dia = 10; clientes[7].fechaAlta.mes = 4; clientes[7].fechaAlta.anio = 2026;
    clientes[7].estado = CLIENTE_ACTIVO;

    // Cliente 9
    strcpy(clientes[8].dni, "33999000");
    strcpy(clientes[8].nombre, "Duarte, Jorge");
    strcpy(clientes[8].telefono, "3764990011");
    strcpy(clientes[8].email, "jorge_duarte@email.com");
    strcpy(clientes[8].direccion, "Sarmiento 1150");
    clientes[8].fechaAlta.dia = 15; clientes[8].fechaAlta.mes = 4; clientes[8].fechaAlta.anio = 2026;
    clientes[8].estado = CLIENTE_INACTIVO;

    // Cliente 10
    strcpy(clientes[9].dni, "44000111");
    strcpy(clientes[9].nombre, "Rojas, Valeria");
    strcpy(clientes[9].telefono, "3764001122");
    strcpy(clientes[9].email, "valeria_rojas@email.com");
    strcpy(clientes[9].direccion, "Buenos Aires 1700");
    clientes[9].fechaAlta.dia = 18; clientes[9].fechaAlta.mes = 4; clientes[9].fechaAlta.anio = 2026;
    clientes[9].estado = CLIENTE_ACTIVO;

    // Asegurarse de que exista la carpeta 'datos' o manejarlo mediante el sistema operativo; 
    // aquí guardamos asumiendo que la carpeta 'datos' ya está creada en tu directorio.
    archivo = fopen("datos/clientes.dat", "wb");
    if (archivo == NULL) {
        printf("Error: No se pudo crear 'datos/clientes.dat'. Asegurate de que la carpeta 'datos' exista.\n");
        return 1;
    }
    fwrite(clientes, sizeof(Cliente), 10, archivo);
    fclose(archivo);
    printf("-> Archivo 'datos/clientes.dat' generado con 10 registros exitosamente.\n");


    /* --- 2. CREAR VEHÍCULOS (10 registros) --- */
    memset(vehiculos, 0, sizeof(vehiculos));

    strcpy(vehiculos[0].patente, "AA123BB");
    strcpy(vehiculos[0].marca, "Fiat");
    strcpy(vehiculos[0].modelo, "Argo");
    vehiculos[0].anio = 2023;
    strcpy(vehiculos[0].tipo, "Automovil");
    vehiculos[0].capacidad = 5;
    vehiculos[0].precioDia = 18500.0f;
    vehiculos[0].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[1].patente, "AB456CD");
    strcpy(vehiculos[1].marca, "Chevrolet");
    strcpy(vehiculos[1].modelo, "Onix");
    vehiculos[1].anio = 2022;
    strcpy(vehiculos[1].tipo, "Automovil");
    vehiculos[1].capacidad = 5;
    vehiculos[1].precioDia = 17000.0f;
    vehiculos[1].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[2].patente, "AC789EF");
    strcpy(vehiculos[2].marca, "Volkswagen");
    strcpy(vehiculos[2].modelo, "Gol Trend");
    vehiculos[2].anio = 2021;
    strcpy(vehiculos[2].tipo, "Automovil");
    vehiculos[2].capacidad = 5;
    vehiculos[2].precioDia = 15000.0f;
    vehiculos[2].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[3].patente, "AD012GH");
    strcpy(vehiculos[3].marca, "Toyota");
    strcpy(vehiculos[3].modelo, "Etios");
    vehiculos[3].anio = 2023;
    strcpy(vehiculos[3].tipo, "Automovil");
    vehiculos[3].capacidad = 5;
    vehiculos[3].precioDia = 19000.0f;
    vehiculos[3].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[4].patente, "AE345IJ");
    strcpy(vehiculos[4].marca, "Renault");
    strcpy(vehiculos[4].modelo, "Sandero");
    vehiculos[4].anio = 2022;
    strcpy(vehiculos[4].tipo, "Automovil");
    vehiculos[4].capacidad = 5;
    vehiculos[4].precioDia = 16500.0f;
    vehiculos[4].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[5].patente, "AF678KL");
    strcpy(vehiculos[5].marca, "Ford");
    strcpy(vehiculos[5].modelo, "EcoSport");
    vehiculos[5].anio = 2021;
    strcpy(vehiculos[5].tipo, "SUV");
    vehiculos[5].capacidad = 5;
    vehiculos[5].precioDia = 24000.0f;
    vehiculos[5].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[6].patente, "AG901MN");
    strcpy(vehiculos[6].marca, "Toyota");
    strcpy(vehiculos[6].modelo, "Hilux");
    vehiculos[6].anio = 2024;
    strcpy(vehiculos[6].tipo, "Camioneta");
    vehiculos[6].capacidad = 5;
    vehiculos[6].precioDia = 45000.0f;
    vehiculos[6].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[7].patente, "AH234OP");
    strcpy(vehiculos[7].marca, "Volkswagen");
    strcpy(vehiculos[7].modelo, "Amarok");
    vehiculos[7].anio = 2023;
    strcpy(vehiculos[7].tipo, "Camioneta");
    vehiculos[7].capacidad = 5;
    vehiculos[7].precioDia = 48000.0f;
    vehiculos[7].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[8].patente, "AI567QR");
    strcpy(vehiculos[8].marca, "Peugeot");
    strcpy(vehiculos[8].modelo, "208");
    vehiculos[8].anio = 2024;
    strcpy(vehiculos[8].tipo, "Automovil");
    vehiculos[8].capacidad = 5;
    vehiculos[8].precioDia = 21000.0f;
    vehiculos[8].estado = VEHICULO_DISPONIBLE;

    strcpy(vehiculos[9].patente, "AJ890ST");
    strcpy(vehiculos[9].marca, "Fiat");
    strcpy(vehiculos[9].modelo, "Cronos");
    vehiculos[9].anio = 2023;
    strcpy(vehiculos[9].tipo, "Automovil");
    vehiculos[9].capacidad = 5;
    vehiculos[9].precioDia = 19500.0f;
    vehiculos[9].estado = VEHICULO_DISPONIBLE;

    archivo = fopen("datos/vehiculos.dat", "wb");
    if (archivo == NULL) {
        printf("Error: No se pudo crear 'datos/vehiculos.dat'.\n");
        return 1;
    }
    fwrite(vehiculos, sizeof(Vehiculo), 10, archivo);
    fclose(archivo);
    printf("-> Archivo 'datos/vehiculos.dat' generado con 10 registros exitosamente.\n");

    printf("\n¡Base de datos de prueba inicializada con exito!\n");
    return 0;
}