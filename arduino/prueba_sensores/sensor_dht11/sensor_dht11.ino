#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN 9

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 2 segundos
  delay(2000);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if( isnan(humidity) || isnan(temperature)){
    Serial.println("ERROR EN EL SENSOR!!!");
    return;
  }

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print(" % Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
}
