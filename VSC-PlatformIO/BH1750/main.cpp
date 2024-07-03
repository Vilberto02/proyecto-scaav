#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 Luxometro;

float lux;

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    Luxometro.begin();
    Serial.println("BH1750 inicializado");
}

void loop()
{
    // put your main code here, to run repeatedly:
    lux = Luxometro.readLightLevel();
    Serial.print("Intensidad de luz: ");
    Serial.print(lux);
    Serial.println(" lx");
    delay(1000);
}
