#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

BH1750 Luxometro;

#define DHTTYPE DHT11
#define DHTPIN 5
#define MQ3PIN A1
#define MQ135PIN A0

DHT dht(DHTPIN, DHTTYPE);

float alcohol_value;
int limit_value = 600;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    dht.begin();
    Luxometro.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
    
    // Variables 
    uint16_t lux = Luxometro.readLightLevel();      // Sensor de intensidad de luz
    int sensor_mq135 = analogRead(MQ135PIN);              // Sensor de CO2
    float humidity = dht.readHumidity();            // Sensor de humedad
    float temperature = dht.readTemperature();      // Sensor de temperatura
    alcohol_value = analogRead(MQ3PIN);                 // Sensor de alcohol
    
    // Imprimir valores en el monitor serial
    Serial.print("Valor del sensor de MQ-135 (CO2): ");
    Serial.print(sensor_mq135);
    Serial.println(" ppm");
    
    Serial.print("Valor del sensor de BH1750 (Intensidad de luz): ");
    Serial.print(lux);
    Serial.println(" lx");
    
    Serial.print("Valor del sensor DHT11 (Humedad): ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Valor del sensor DHT11 (Temperatura): ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Valor del sensor MQ-3 (Alcohol): ");
    Serial.print(alcohol_value);
    Serial.println(" ppm");
    Serial.println();

    delay(1000);
}
