typedef struct{
    char date[11];
    float pm25;
    float pm10;
    float co;
    float no2;
    float so2;
    float o3;
    float temperature;
    float humidity;
    float windSpeed;
} Medition;



typedef struct{
    char zone[50];
    Medition zoneMedition ;

}Zone;

