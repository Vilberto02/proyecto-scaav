float alcohol_value;
int limit_value = 600;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  alcohol_value = analogRead(A1);
  Serial.print("Valor de alcohol: ");
  Serial.print(alcohol_value);
  float percent = (alcohol_value/10000);
  Serial.print("\nPorcentaje: ");
  Serial.print(percent);
  Serial.println(" %");

  if (alcohol_value > limit_value){
    digitalWrite(13, HIGH);
  }else{
    digitalWrite(13, LOW);
  }
  delay(400);
}
