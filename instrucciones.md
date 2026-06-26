# Documento de Planificación: Sistema Integral de Gestión y Predicción de Contaminación del Aire
**Rol:** Ingeniero de Software Principal
**Lenguaje:** C (Estándar ISO C99/C11)
**Restricciones:** Uso exclusivo de librerías estándar (`stdio.h`, `stdlib.h`, `string.h`), sin librerías de terceros.

---

## 1. Visión General de la Arquitectura
El sistema se construirá utilizando una arquitectura modular basada en funciones, estructuras de datos estáticas (para almacenar las métricas de las 5 zonas) y manejo de punteros para la eficiencia de memoria. La persistencia se logrará mediante archivos binarios o de texto plano (`.dat` o `.txt`) para lectura y escritura de históricos y límites.

---

## 2. Definición de Estructuras de Datos (`structs`)
Para cumplir con los requerimientos, modelaremos la realidad con las siguientes estructuras centrales:

```c
// Definiciones preprocesador
#define NUM_ZONAS 5
#define DIAS_HISTORICOS 30

// Estructura para factores climáticos
typedef struct {
    float temperatura;
    float velocidad_viento;
    float humedad;
} Clima;

// Estructura para los contaminantes
typedef struct {
    float co2;
    float so2;
    float no2;
    float pm25;
    float pm10;
    float o3;
} Contaminantes;

// Estructura para los límites aceptables (Módulo 1)
typedef struct {
    Contaminantes limites_oms;
    Contaminantes limites_locales;
} LimitesAceptables;

// Estructura principal por Zona y Día (Módulo 2)
typedef struct {
    int id_zona;
    char nombre_zona[50];
    Contaminantes niveles_actuales;
    Clima condiciones_climaticas;
    float ica_general; // Calculado con la fórmula
} RegistroDiario;