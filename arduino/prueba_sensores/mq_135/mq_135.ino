void setup() {
  Serial.begin(9600);
}

void loop() {
  int s_analogica_mq135 = analogRead(A0);
  
  Serial.print("Valor: ");
  Serial.print(s_analogica_mq135);
  Serial.println(" ppm");
  
  // 1 segundo
  delay(1000);
}
