#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>

#define NUM_ZONAS 5
#define DIAS_HISTORICOS 30
#define LONGITUD_NOMBRE 50
#define LONGITUD_FECHA 11

typedef struct {
    float temperatura;
    float velocidad_viento;
    float humedad;
} Clima;

typedef struct {
    float co2;
    float so2;
    float no2;
    float pm25;
    float pm10;
    float o3;
} Contaminantes;

typedef struct {
    Contaminantes limites_oms;
    Contaminantes limites_locales;
} LimitesAceptables;

typedef struct {
    int id_zona;
    char nombre_zona[LONGITUD_NOMBRE];
    char fecha[LONGITUD_FECHA];
    Contaminantes niveles_actuales;
    Clima condiciones_climaticas;
    float ica_general;
} RegistroDiario;

typedef struct {
    char nombre_zona[LONGITUD_NOMBRE];
    RegistroDiario dias[DIAS_HISTORICOS];
    int dias_registrados;
} ZonaHistorico;

void inicializarZonas(ZonaHistorico zonas[]);
void inicializarLimites(LimitesAceptables *limites);
float calcularICA(const Contaminantes *niveles);
void actualizarICA(RegistroDiario *registro);
int guardarHistorico(const char *ruta, const ZonaHistorico zonas[]);
int cargarHistorico(const char *ruta, ZonaHistorico zonas[]);
int guardarLimites(const char *ruta, const LimitesAceptables *limites);
int cargarLimites(const char *ruta, LimitesAceptables *limites);
int estaFueraDeLimites(const Contaminantes *valores, const LimitesAceptables *limites, char *reporte, size_t maxlen);
void mostrarRegistro(const RegistroDiario *registro);
void mostrarZonaHistorico(const ZonaHistorico *zona);
void predecirCalidad(const ZonaHistorico *zona, const LimitesAceptables *limites);
void registrarMedicion(ZonaHistorico zonas[], int indiceZona, const RegistroDiario *registro);
int leerEntero(int *valor, const char *prompt);
int leerFlotante(float *valor, const char *prompt);
int leerCadena(char *dest, size_t size, const char *prompt);

#endif // FUNCTIONS_H

