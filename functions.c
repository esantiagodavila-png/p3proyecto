#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

static const char *zonas_defecto[NUM_ZONAS] = {
    "Norte",
    "Sur",
    "Este",
    "Oeste",
    "Centro"
};

void inicializarZonas(ZonaHistorico zonas[]) {
    for (int i = 0; i < NUM_ZONAS; i++) {
        strncpy(zonas[i].nombre_zona, zonas_defecto[i], LONGITUD_NOMBRE - 1);
        zonas[i].nombre_zona[LONGITUD_NOMBRE - 1] = '\0';
        zonas[i].dias_registrados = 0;
        for (int j = 0; j < DIAS_HISTORICOS; j++) {
            zonas[i].dias[j].id_zona = i + 1;
            zonas[i].dias[j].nombre_zona[0] = '\0';
            zonas[i].dias[j].fecha[0] = '\0';
            zonas[i].dias[j].ica_general = 0.0f;
            memset(&zonas[i].dias[j].niveles_actuales, 0, sizeof(Contaminantes));
            memset(&zonas[i].dias[j].condiciones_climaticas, 0, sizeof(Clima));
        }
    }
}

void inicializarLimites(LimitesAceptables *limites) {
    limites->limites_oms.co2 = 1000.0f;
    limites->limites_oms.so2 = 20.0f;
    limites->limites_oms.no2 = 40.0f;
    limites->limites_oms.pm25 = 25.0f;
    limites->limites_oms.pm10 = 50.0f;
    limites->limites_oms.o3 = 100.0f;

    limites->limites_locales.co2 = 900.0f;
    limites->limites_locales.so2 = 10.0f;
    limites->limites_locales.no2 = 30.0f;
    limites->limites_locales.pm25 = 20.0f;
    limites->limites_locales.pm10 = 40.0f;
    limites->limites_locales.o3 = 90.0f;
}

static float calcularICAComponente(float valor, float limite) {
    if (limite <= 0.0f) {
        return 0.0f;
    }
    return (valor / limite) * 100.0f;
}

float calcularICA(const Contaminantes *niveles) {
    float suma = 0.0f;
    float contador = 0.0f;

    suma += calcularICAComponente(niveles->pm25, 25.0f);
    contador += 1.0f;
    suma += calcularICAComponente(niveles->pm10, 50.0f);
    contador += 1.0f;
    suma += calcularICAComponente(niveles->o3, 100.0f);
    contador += 1.0f;
    suma += calcularICAComponente(niveles->no2, 40.0f);
    contador += 1.0f;
    suma += calcularICAComponente(niveles->so2, 20.0f);
    contador += 1.0f;
    suma += calcularICAComponente(niveles->co2, 1000.0f);
    contador += 1.0f;

    if (contador == 0.0f) {
        return 0.0f;
    }

    return suma / contador;
}

void actualizarICA(RegistroDiario *registro) {
    registro->ica_general = calcularICA(&registro->niveles_actuales);
}

int guardarHistorico(const char *ruta, const ZonaHistorico zonas[]) {
    FILE *archivo = fopen(ruta, "wb");
    if (!archivo) {
        return 0;
    }
    size_t escritos = fwrite(zonas, sizeof(ZonaHistorico), NUM_ZONAS, archivo);
    fclose(archivo);
    return escritos == NUM_ZONAS;
}

int cargarHistorico(const char *ruta, ZonaHistorico zonas[]) {
    FILE *archivo = fopen(ruta, "rb");
    if (!archivo) {
        return 0;
    }
    size_t leidos = fread(zonas, sizeof(ZonaHistorico), NUM_ZONAS, archivo);
    fclose(archivo);
    return leidos == NUM_ZONAS;
}

int guardarLimites(const char *ruta, const LimitesAceptables *limites) {
    FILE *archivo = fopen(ruta, "w");
    if (!archivo) {
        return 0;
    }
    fprintf(archivo, "%f %f %f %f %f %f\n",
            limites->limites_oms.co2,
            limites->limites_oms.so2,
            limites->limites_oms.no2,
            limites->limites_oms.pm25,
            limites->limites_oms.pm10,
            limites->limites_oms.o3);
    fprintf(archivo, "%f %f %f %f %f %f\n",
            limites->limites_locales.co2,
            limites->limites_locales.so2,
            limites->limites_locales.no2,
            limites->limites_locales.pm25,
            limites->limites_locales.pm10,
            limites->limites_locales.o3);
    fclose(archivo);
    return 1;
}

int cargarLimites(const char *ruta, LimitesAceptables *limites) {
    FILE *archivo = fopen(ruta, "r");
    if (!archivo) {
        return 0;
    }
    int leidos = fscanf(archivo, "%f %f %f %f %f %f\n",
                        &limites->limites_oms.co2,
                        &limites->limites_oms.so2,
                        &limites->limites_oms.no2,
                        &limites->limites_oms.pm25,
                        &limites->limites_oms.pm10,
                        &limites->limites_oms.o3);
    if (leidos != 6) {
        fclose(archivo);
        return 0;
    }
    leidos = fscanf(archivo, "%f %f %f %f %f %f\n",
                    &limites->limites_locales.co2,
                    &limites->limites_locales.so2,
                    &limites->limites_locales.no2,
                    &limites->limites_locales.pm25,
                    &limites->limites_locales.pm10,
                    &limites->limites_locales.o3);
    fclose(archivo);
    return leidos == 6;
}

int estaFueraDeLimites(const Contaminantes *valores, const LimitesAceptables *limites, char *reporte, size_t maxlen) {
    int fuera = 0;
    reporte[0] = '\0';

    if (valores->pm25 > limites->limites_locales.pm25) {
        fuera = 1;
        strncat(reporte, "PM2.5 fuera del limite local. ", maxlen - strlen(reporte) - 1);
    } else if (valores->pm25 > limites->limites_oms.pm25) {
        fuera = 1;
        strncat(reporte, "PM2.5 fuera del limite OMS. ", maxlen - strlen(reporte) - 1);
    }

    if (valores->pm10 > limites->limites_locales.pm10) {
        fuera = 1;
        strncat(reporte, "PM10 fuera del limite local. ", maxlen - strlen(reporte) - 1);
    } else if (valores->pm10 > limites->limites_oms.pm10) {
        fuera = 1;
        strncat(reporte, "PM10 fuera del limite OMS. ", maxlen - strlen(reporte) - 1);
    }

    if (valores->o3 > limites->limites_locales.o3) {
        fuera = 1;
        strncat(reporte, "O3 fuera del limite local. ", maxlen - strlen(reporte) - 1);
    } else if (valores->o3 > limites->limites_oms.o3) {
        fuera = 1;
        strncat(reporte, "O3 fuera del limite OMS. ", maxlen - strlen(reporte) - 1);
    }

    if (valores->no2 > limites->limites_locales.no2) {
        fuera = 1;
        strncat(reporte, "NO2 fuera del limite local. ", maxlen - strlen(reporte) - 1);
    } else if (valores->no2 > limites->limites_oms.no2) {
        fuera = 1;
        strncat(reporte, "NO2 fuera del limite OMS. ", maxlen - strlen(reporte) - 1);
    }

    if (valores->so2 > limites->limites_locales.so2) {
        fuera = 1;
        strncat(reporte, "SO2 fuera del limite local. ", maxlen - strlen(reporte) - 1);
    } else if (valores->so2 > limites->limites_oms.so2) {
        fuera = 1;
        strncat(reporte, "SO2 fuera del limite OMS. ", maxlen - strlen(reporte) - 1);
    }

    if (valores->co2 > limites->limites_locales.co2) {
        fuera = 1;
        strncat(reporte, "CO2 fuera del limite local. ", maxlen - strlen(reporte) - 1);
    } else if (valores->co2 > limites->limites_oms.co2) {
        fuera = 1;
        strncat(reporte, "CO2 fuera del limite OMS. ", maxlen - strlen(reporte) - 1);
    }

    return fuera;
}

void mostrarRegistro(const RegistroDiario *registro) {
    printf("%s | %s | ICA %.2f\n",
           registro->nombre_zona,
           registro->fecha,
           registro->ica_general);
    printf("  PM2.5: %.2f  PM10: %.2f  O3: %.2f\n",
           registro->niveles_actuales.pm25,
           registro->niveles_actuales.pm10,
           registro->niveles_actuales.o3);
    printf("  NO2: %.2f  SO2: %.2f  CO2: %.2f\n",
           registro->niveles_actuales.no2,
           registro->niveles_actuales.so2,
           registro->niveles_actuales.co2);
    printf("  Temp: %.2f°C  Humedad: %.2f%%  Viento: %.2f km/h\n",
           registro->condiciones_climaticas.temperatura,
           registro->condiciones_climaticas.humedad,
           registro->condiciones_climaticas.velocidad_viento);
}

void mostrarZonaHistorico(const ZonaHistorico *zona) {
    printf("\nHistorial de zona: %s (%d registros)\n",
           zona->nombre_zona,
           zona->dias_registrados);
    printf("----------------------------------------------------\n");
    for (int i = 0; i < zona->dias_registrados; i++) {
        mostrarRegistro(&zona->dias[i]);
        printf("----------------------------------------------------\n");
    }
}

void predecirCalidad(const ZonaHistorico *zona, const LimitesAceptables *limites) {
    if (zona->dias_registrados == 0) {
        printf("No hay datos historicos para la zona %s.\n", zona->nombre_zona);
        return;
    }

    int dias_considerados = zona->dias_registrados < 3 ? zona->dias_registrados : 3;
    Contaminantes promedio = {0};

    for (int i = zona->dias_registrados - dias_considerados; i < zona->dias_registrados; i++) {
        promedio.co2 += zona->dias[i].niveles_actuales.co2;
        promedio.so2 += zona->dias[i].niveles_actuales.so2;
        promedio.no2 += zona->dias[i].niveles_actuales.no2;
        promedio.pm25 += zona->dias[i].niveles_actuales.pm25;
        promedio.pm10 += zona->dias[i].niveles_actuales.pm10;
        promedio.o3 += zona->dias[i].niveles_actuales.o3;
    }

    promedio.co2 /= dias_considerados;
    promedio.so2 /= dias_considerados;
    promedio.no2 /= dias_considerados;
    promedio.pm25 /= dias_considerados;
    promedio.pm10 /= dias_considerados;
    promedio.o3 /= dias_considerados;

    float ica_predicho = calcularICA(&promedio);
    char reporte[256];
    int fuera = estaFueraDeLimites(&promedio, limites, reporte, sizeof(reporte));

    printf("\nPrediccion para la zona %s basada en los ultimos %d dias:\n",
           zona->nombre_zona,
           dias_considerados);
    printf("  PM2.5 estimado: %.2f\n", promedio.pm25);
    printf("  PM10 estimado: %.2f\n", promedio.pm10);
    printf("  O3 estimado: %.2f\n", promedio.o3);
    printf("  NO2 estimado: %.2f\n", promedio.no2);
    printf("  SO2 estimado: %.2f\n", promedio.so2);
    printf("  CO2 estimado: %.2f\n", promedio.co2);
    printf("  ICA estimado: %.2f\n", ica_predicho);

    if (fuera) {
        printf("  Alerta: valores fuera de limites: %s\n", reporte);
    } else {
        printf("  Valores estimados dentro de los limites locales y OMS.\n");
    }
}

void registrarMedicion(ZonaHistorico zonas[], int indiceZona, const RegistroDiario *registro) {
    if (indiceZona < 0 || indiceZona >= NUM_ZONAS || registro == NULL) {
        return;
    }

    ZonaHistorico *zona = &zonas[indiceZona];
    RegistroDiario copia = *registro;
    copia.id_zona = indiceZona + 1;
    strncpy(copia.nombre_zona, zona->nombre_zona, LONGITUD_NOMBRE - 1);
    copia.nombre_zona[LONGITUD_NOMBRE - 1] = '\0';
    actualizarICA(&copia);

    int posicion = zona->dias_registrados % DIAS_HISTORICOS;
    zona->dias[posicion] = copia;

    if (zona->dias_registrados < DIAS_HISTORICOS) {
        zona->dias_registrados += 1;
    }
}

int leerEntero(int *valor, const char *prompt) {
    char buffer[64];
    if (prompt && prompt[0]) {
        printf("%s", prompt);
    }
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return 0;
    }
    *valor = atoi(buffer);
    return 1;
}

int leerFlotante(float *valor, const char *prompt) {
    char buffer[64];
    if (prompt && prompt[0]) {
        printf("%s", prompt);
    }
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return 0;
    }
    *valor = strtof(buffer, NULL);
    return 1;
}

int leerCadena(char *dest, size_t size, const char *prompt) {
    if (prompt && prompt[0]) {
        printf("%s", prompt);
    }
    if (!fgets(dest, (int)size, stdin)) {
        return 0;
    }
    size_t longitud = strcspn(dest, "\r\n");
    dest[longitud] = '\0';
    return 1;
}




