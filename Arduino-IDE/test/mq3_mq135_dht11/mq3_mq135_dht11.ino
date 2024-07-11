#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN 9

DHT dht(DHTPIN, DHTTYPE);
   
float valor_alcohol;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 0.4 segundos
  delay(3000);
  valor_alcohol = analogRead(A1);
  float percent = (valor_alcohol/100);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int s_analogica_mq135 = analogRead(A0);

  Serial.println("\n--- Sensor MQ-135 ---");
  Serial.print("CO2 Valor: ");
  Serial.print(s_analogica_mq135);
  Serial.println(" ppm");
  Serial.println();
  Serial.println("\n--- Sensor MQ-3 ---");
  Serial.print("Nivel de alcohol: ");
  Serial.println(valor_alcohol);
  Serial.print("Porcentaje de alcohol: ");
  Serial.print(percent);
  Serial.print(" %");

  if( isnan(humidity) || isnan(temperature)){
    Serial.println("ERROR EN EL SENSOR!!!");
    return;
  }

  Serial.println("\n--- Sensor DHT11 ---");
  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print(" % Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.println();
}
