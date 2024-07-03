/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL2LU994cah"
#define BLYNK_TEMPLATE_NAME "PruebaDHT"
#define BLYNK_AUTH_TOKEN "TTBmyT-w_ja47rwDNZ8HS3YUsIu7l-3s"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Patricio - 2.4G";
char pass[] = "7652990024";

#define DHTPIN 15

DHTesp dht;
float temperature;
float humidity;

BlynkTimer timer;

void sendSensor()
{
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    Serial.print("Temperatura: ");
    Serial.println(temperature);
    Serial.print("Humedad: ");
    Serial.println(humidity);
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
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

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    // You can also specify server:
    // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
    // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

    dht.setup(DHTPIN, DHTesp::DHT11);
    Serial.println("Sensor DHT11 inicializado");
    // Setup a function to be called every second
    timer.setInterval(1000L, sendSensor);
}

void loop()
{
    Blynk.run();
    timer.run();
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!
}
