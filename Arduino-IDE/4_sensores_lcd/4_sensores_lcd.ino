#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

BH1750 Luxometro;
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT11
#define DHTPIN 5
#define MQ135PIN A0
#define MQ3PIN A1


DHT dht(DHTPIN, DHTTYPE);

float alcohol_value;
uint16_t lux;
int sensor_mq135;
float humidity;
float temperature;
double coppm;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Wire.begin();
    dht.begin();
    Luxometro.begin();
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void loop() {
    // put your main code here, to run repeatedly:
    
    // Variables 
    lux = Luxometro.readLightLevel();      // Sensor de intensidad de luz
    sensor_mq135 = analogRead(MQ135PIN);              // Sensor de CO2
    humidity = dht.readHumidity();            // Sensor de humedad
    temperature = dht.readTemperature();      // Sensor de temperatura
    alcohol_value = analogRead(MQ3PIN);                 // Sensor de alcohol

    float tension = sensor_mq135 * (5.0 / 1023.0);
    float resistencia = 1000 * ((5 - tension) / tension);
    coppm = 245 * pow(resistencia / 5463, -2.26);

    lcd.clear();

    // Imprimir valores en el display
    lcd.setCursor(0, 0);
    lcd.print("HT:");
    lcd.print(humidity, 0);
    lcd.print(" ");
    lcd.print(temperature, 0);
    lcd.print(" ");

    lcd.print("L:");
    lcd.print(lux);

    lcd.setCursor(0, 1);
    lcd.print("C:");
    lcd.print(coppm, 1);
    lcd.print(" ");
    lcd.print("A:");
    lcd.print(alcohol_value, 1);
    delay(3000);
}

void mostrarValoresSerial(){
  // Imprimir valores en el monitor serial
    Serial.print("Valor del sensor de MQ-135 (CO2): ");
    Serial.print(coppm);
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
}
