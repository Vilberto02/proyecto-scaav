#include <Wire.h>
#include <BH1750.h>

BH1750 Luxometro;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Inicializando sensor...");
  Luxometro.begin(); // Se conecta automáticamente
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t lux = Luxometro.readLightLevel();
  Serial.print("Luz (luminancia): ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(500);
}
