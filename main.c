#include <stdio.h>
#include <string.h>
#include "functions.h"

static const char ARCHIVO_HISTORICO[] = "contaminacionzonas.dat";
static const char ARCHIVO_LIMITES[] = "limitescontaminacion.txt";

static void mostrarMenu(void) {
    printf("\n=== GESTION Y PREDICCION DE CONTAMINACION ===\n");
    printf("1. Cargar historico desde archivo\n");
    printf("2. Guardar historico en archivo\n");
    printf("3. Cargar limites aceptables\n");
    printf("4. Guardar limites aceptables\n");
    printf("5. Registrar nueva medicion\n");
    printf("6. Mostrar historial de zona\n");
    printf("7. Mostrar limites actuales\n");
    printf("8. Generar prediccion de calidad\n");
    printf("9. Salir\n");
    printf("Seleccione una opcion: ");
}

static int seleccionarZona(void) {
    printf("Seleccione una zona (1-%d):\n", NUM_ZONAS);
    for (int i = 0; i < NUM_ZONAS; i++) {
        printf("%d. Zona %d\n", i + 1, i + 1);
    }
    int opcion = 0;
    if (!leerEntero(&opcion, "Opcion: ")) {
        return -1;
    }
    if (opcion < 1 || opcion > NUM_ZONAS) {
        return -1;
    }
    return opcion - 1;
}

static void imprimirLimites(const LimitesAceptables *limites) {
    printf("\nLimites OMS:\n");
    printf("  CO2: %.2f  SO2: %.2f  NO2: %.2f  PM2.5: %.2f  PM10: %.2f  O3: %.2f\n",
           limites->limites_oms.co2,
           limites->limites_oms.so2,
           limites->limites_oms.no2,
           limites->limites_oms.pm25,
           limites->limites_oms.pm10,
           limites->limites_oms.o3);
    printf("Limites locales:\n");
    printf("  CO2: %.2f  SO2: %.2f  NO2: %.2f  PM2.5: %.2f  PM10: %.2f  O3: %.2f\n",
           limites->limites_locales.co2,
           limites->limites_locales.so2,
           limites->limites_locales.no2,
           limites->limites_locales.pm25,
           limites->limites_locales.pm10,
           limites->limites_locales.o3);
}

static void registrarNuevaMedicion(ZonaHistorico zonas[]) {
    int indice = seleccionarZona();
    if (indice < 0) {
        printf("Seleccion invalida.\n");
        return;
    }

    RegistroDiario registro;
    char temp[LONGITUD_FECHA];

    if (!leerCadena(temp, sizeof(temp), "Fecha (YYYY-MM-DD): ")) {
        printf("Error leyendo la fecha.\n");
        return;
    }
    strncpy(registro.fecha, temp, LONGITUD_FECHA - 1);
    registro.fecha[LONGITUD_FECHA - 1] = '\0';
    registro.id_zona = indice + 1;
    strncpy(registro.nombre_zona, zonas[indice].nombre_zona, LONGITUD_NOMBRE - 1);
    registro.nombre_zona[LONGITUD_NOMBRE - 1] = '\0';

    leerFlotante(&registro.niveles_actuales.pm25, "PM2.5: ");
    leerFlotante(&registro.niveles_actuales.pm10, "PM10: ");
    leerFlotante(&registro.niveles_actuales.o3, "O3: ");
    leerFlotante(&registro.niveles_actuales.no2, "NO2: ");
    leerFlotante(&registro.niveles_actuales.so2, "SO2: ");
    leerFlotante(&registro.niveles_actuales.co2, "CO2: ");
    leerFlotante(&registro.condiciones_climaticas.temperatura, "Temperatura (C): ");
    leerFlotante(&registro.condiciones_climaticas.humedad, "Humedad (%%): ");
    leerFlotante(&registro.condiciones_climaticas.velocidad_viento, "Velocidad de viento (km/h): ");

    registrarMedicion(zonas, indice, &registro);
    printf("Medicion registrada en la zona %s.\n", zonas[indice].nombre_zona);
}

int main(void) {
    ZonaHistorico zonas[NUM_ZONAS];
    LimitesAceptables limites;
    inicializarZonas(zonas);
    inicializarLimites(&limites);

    int opcion = 0;
    while (1) {
        mostrarMenu();
        if (!leerEntero(&opcion, "")) {
            printf("Entrada invalida.\n");
            continue;
        }

        switch (opcion) {
            case 1:
                if (cargarHistorico(ARCHIVO_HISTORICO, zonas)) {
                    printf("Historico cargado correctamente desde %s.\n", ARCHIVO_HISTORICO);
                } else {
                    printf("No se pudo cargar el historico. Se conservan los datos en memoria.\n");
                }
                break;
            case 2:
                if (guardarHistorico(ARCHIVO_HISTORICO, zonas)) {
                    printf("Historico guardado correctamente en %s.\n", ARCHIVO_HISTORICO);
                } else {
                    printf("Error guardando el historico.\n");
                }
                break;
            case 3:
                if (cargarLimites(ARCHIVO_LIMITES, &limites)) {
                    printf("Limites cargados desde %s.\n", ARCHIVO_LIMITES);
                } else {
                    printf("No se pudo cargar los limites. Se usan valores por defecto.\n");
                }
                break;
            case 4:
                if (guardarLimites(ARCHIVO_LIMITES, &limites)) {
                    printf("Limites guardados en %s.\n", ARCHIVO_LIMITES);
                } else {
                    printf("Error guardando los limites.\n");
                }
                break;
            case 5:
                registrarNuevaMedicion(zonas);
                break;
            case 6: {
                int indice = seleccionarZona();
                if (indice < 0) {
                    printf("Seleccion invalida.\n");
                } else {
                    mostrarZonaHistorico(&zonas[indice]);
                }
                break;
            }
            case 7:
                imprimirLimites(&limites);
                break;
            case 8: {
                int indice = seleccionarZona();
                if (indice < 0) {
                    printf("Seleccion invalida.\n");
                } else {
                    predecirCalidad(&zonas[indice], &limites);
                }
                break;
            }
            case 9:
                printf("Saliendo...\n");
                return 0;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
                break;
        }
    }

    return 0;
}
