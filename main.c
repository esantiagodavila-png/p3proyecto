#include <stdio.h> 
#include <string.h>
#include "functions.h"


int main(){
 
   FILE *f = fopen("contaminacionzonas.dat", "rb");
Zone z;

while (fread(&z, sizeof(Zone), 1, f) == 1) {
    printf("%-12s %-10s %.2f %.2f %.2f\n",
           z.zone,
           z.zoneMedition.date,
           z.zoneMedition.pm25,
           z.zoneMedition.pm10,
           z.zoneMedition.temperature);
}

fclose(f);
};