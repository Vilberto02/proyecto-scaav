#include <Arduino.h>
#include "DHTesp.h"

DHTesp dht;

#define PINDHT 15

void setup()
{
    Serial.begin(115200);
    Serial.println("Probando el DHT11 en el ESP32");
    dht.setup(PINDHT, DHTesp::DHT11);
    Serial.println("DHT11 inicializado");
}

void loop()
{
    // put your main code here, to run repeatedly:
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    Serial.print("Temperatura: ");
    Serial.println(temperature);
    Serial.print("Humedad: ");
    Serial.println(humidity);
    delay(1000);
}