/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL21ggGuLGD"
#define BLYNK_TEMPLATE_NAME "SCAAV"
#define BLYNK_AUTH_TOKEN "nwvCwfBcaq-hJPClxtOp-PCF0foXtA1c"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>

#include <Wire.h>
#include <BH1750.h>
//  Your WiFi credentials.
//  Set password to "" for open networks.
char ssid[] = "RedmiNote11";
char pass[] = "76529900";

#define DHTPIN 4
#define MQ135PIN 32
#define MQ3PIN 33

DHTesp dht;
BH1750 luxometro;

float temperature;
float humidity;
float alcohol;
float dioxidoCarbono;
uint16_t intensidadLuz;

BlynkTimer timer;

void sendAll();

void sendDHT11()
{
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    Blynk.virtualWrite(V0, humidity);
    Blynk.virtualWrite(V1, temperature);
}

void sendBH1750()
{
    intensidadLuz = luxometro.readLightLevel();
    Serial.print("Intensidad de luz: ");
    Serial.print(intensidadLuz);
    Serial.println(" lux");
    Blynk.virtualWrite(V4, intensidadLuz);
}

void sendMQ135()
{
    dioxidoCarbono = analogRead(MQ135PIN);
    Serial.print("Nivel de CO2: ");
    Serial.print(dioxidoCarbono);
    Serial.println(" ppm");
    Blynk.virtualWrite(V2, dioxidoCarbono);
}

void sendMQ3()
{
    alcohol = analogRead(MQ3PIN);
    Serial.print("Nivel de alcohol: ");
    Serial.print(alcohol);
    Serial.println(" ppm");
    Blynk.virtualWrite(V3, alcohol);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
    // Debug console
    Serial.begin(115200);
    Wire.begin();
    Serial.println("Inicializando la conexion al Wifi y a Blynk");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    // You can also specify server:
    // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
    // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
    luxometro.begin();
    Serial.println("Sensor BH175O inicializado");
    dht.setup(DHTPIN, DHTesp::DHT11);
    Serial.println("Sensor DHT11 inicializado");
    // Setup a function to be called every second
    timer.setInterval(1000L, sendAll);
}

void loop()
{
    Blynk.run();
    timer.run();
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!
}

void sendAll()
{
    sendDHT11();
    sendMQ135();
    sendMQ3();
    sendBH1750();
}
