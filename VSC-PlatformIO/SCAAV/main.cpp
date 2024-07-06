//---------------------------------------- Metadata del dispositivo en Blynk
#define BLYNK_TEMPLATE_ID "TMPL21ggGuLGD"
#define BLYNK_TEMPLATE_NAME "SCAAV"
#define BLYNK_AUTH_TOKEN "nwvCwfBcaq-hJPClxtOp-PCF0foXtA1c"

//---------------------------------------- Librerias
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>
#include <Wire.h>
#include <BH1750.h>

//---------------------------------------- Credenciales para la conexión a internet
char ssid[] = "Patricio - 2.4G";
char pass[] = "7652990024";

//---------------------------------------- Pines de los sensores
#define DHTPIN 4
#define MQ135PIN 32
#define MQ3PIN 33
#define BUZZERPIN 13 // --> Cambiar

//---------------------------------------- Inicializacion de objetos
DHTesp dht;
BH1750 luxometro;

//---------------------------------------- Variables de los sensores
float temperature;
int humidity;
float alcohol;
double dioxidoCarbono;
float intensidadLuz;
double CO2ppm = 0.0;

//---------------------------------------- Umbrales
const float UMBRAL_TEMPERATURA = 30.0;
const int UMBRAL_HUMEDAD = 60;
const float UMBRAL_CO2 = 1000.0;
const float UMBRAL_ALCOHOL = 200.0;
const float UMBRAL_LUZ = 500.0;

//---------------------------------------- Variables para el envio de datos a Google Sheet
String Status_Read_Sensor = "";
String Web_App_URL = "https://script.google.com/macros/s/AKfycbzVJ1kzCxN0LpAfc1YpUmH8JLgWs_K4vQ7laYJWxOgJ2ZAnyDcvg-8XEWp5qVUepCp5/exec"; // Script de Google

// Timer de Blynk
BlynkTimer timer;

//---------------------------------------- Prototipo de las funciones
void sendAllToBlynk();
void calibrateMQ3();
void calibrateMQ135();
void sendDataLCD();
void sendDataGoogleSheet();
void verifyThresholds();

void sendDHT11()
{
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println();
        Serial.println(F("Error en la lectura del sensor DHT11"));
        Serial.println();

        Status_Read_Sensor = "Failed";
        temperature = 0.00;
        humidity = 0;
    }
    else
    {
        Status_Read_Sensor = "Succes";
    }

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

    if (intensidadLuz < 0)
    {
        intensidadLuz = 0;
    }

    Serial.print("Intensidad de luz: ");
    Serial.print(intensidadLuz);
    Serial.println(" lux");
    Blynk.virtualWrite(V4, intensidadLuz);
}

void sendMQ135()
{
    calibrateMQ135();
    // dioxidoCarbono = analogRead(MQ135PIN);
    dioxidoCarbono = CO2ppm;
    Serial.print("Nivel de CO2 calibrado: ");
    Serial.print(dioxidoCarbono);
    Serial.println(" ppm");
    Blynk.virtualWrite(V2, dioxidoCarbono);
}

void calibrateMQ135()
{
    float medidaAnalogica = analogRead(MQ135PIN);
    float tension = medidaAnalogica * (5.0 / 1023.0);
    float resistencia = 1000 * ((5 - tension) / tension);
    CO2ppm = 245 * pow(resistencia / 5463, -2.26); // CO2
    Serial.print("Medida analogica del MQ135: ");
    Serial.print(medidaAnalogica);
    Serial.print(" | Tension: ");
    Serial.print(tension);
    Serial.print(" | resistencia: ");
    Serial.print(resistencia);
    Serial.print(" | CO2ppm: ");
    Serial.print(CO2ppm);
    Serial.println(" ppm");
}

void sendMQ3()
{
    calibrateMQ3();
    alcohol = analogRead(MQ3PIN);
    Serial.print("Nivel de alcohol: ");
    Serial.print(alcohol);
    Serial.println(" ppm");
    Blynk.virtualWrite(V3, alcohol);
}

void calibrateMQ3()
{
    float sensor_volt;
    float RS; //  Get the value of RS via in a clear air
    float R0; // Get the value of R0 via in Alcohol
    float sensorValue = 0.0;

    for (int i = 0; i < 100; i++)
    {
        sensorValue = sensorValue + analogRead(MQ3PIN);
    }

    sensorValue = sensorValue / 100.0; // get average of reading
    sensor_volt = sensorValue / 1024 * 3.0;
    RS = (3.0 - sensor_volt) / sensor_volt; //
    R0 = RS / 60.0;                         // 60 is found using interpolation
    Serial.print("Voltaje del sensor MQ3: ");
    Serial.print(sensor_volt);
    Serial.print(" | Valor del sensor MQ3: ");
    Serial.print(sensorValue);
    Serial.print(" | RS: ");
    Serial.print(RS);
    Serial.print(" | R0: ");
    Serial.print(R0);
    Serial.println(R0);
    // delay(1000);
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

    // pinMode(BUZZERPIN, OUTPUT);
    // digitalWrite(BUZZERPIN, LOW);
    // lcd.begin();
    luxometro.begin();
    Serial.println("Sensor BH175O inicializado");
    dht.setup(DHTPIN, DHTesp::DHT11);
    Serial.println("Sensor DHT11 inicializado");
    // Setup a function to be called every second
    timer.setInterval(1000L, sendAllToBlynk);
    timer.setInterval(120000L, sendDataGoogleSheet);
    // timer.setInterval(1000L, verifyThresholds); // Verificar umbrales cada segundo
}

void loop()
{
    Blynk.run();
    timer.run();
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!
}

//---------------------------------------- Funcion para enviar los datos de los sensores a la aplicación de Blynk
void sendAllToBlynk()
{
    sendDHT11();
    sendMQ135();
    sendMQ3();
    sendBH1750();

    // Mostrar LCD
    // sendDataLCD();
}

//---------------------------------------- Función para mostrar los valores de los sensores en el display LCD
void sendDataLCD()
{
    /*
    Contenido para mostrar valores en el display LCD

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("H:");
    lcd.print(humidity, 0);
    lcd.print(" % T:");
    lcd.print(temperature, 0);
    lcd.print(" C");

    lcd.print("L:");
    lcd.print(intensidadLuz);

    lcd.setCursor(0, 1);
    lcd.print("C:");
    lcd.print(dioxidoCarbono);
    lcd.print(" ");
    lcd.print("A:");
    lcd.print(alcohol, 1);
    */
}

//---------------------------------------- Función para enviar la data a Google Sheet
void sendDataGoogleSheet()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        String Send_Data_URL = Web_App_URL + "?sts=write";
        Send_Data_URL += "&srs=" + Status_Read_Sensor;
        Send_Data_URL += "&temp=" + String(temperature);
        Send_Data_URL += "&humd=" + String(humidity);
        Send_Data_URL += "&co2=" + String(dioxidoCarbono);
        Send_Data_URL += "&alcohol=" + String(alcohol);
        Send_Data_URL += "&light=" + String(intensidadLuz);

        Serial.println();
        Serial.println("-------------");
        Serial.println("Enviando datos a Google Spreadsheet...");
        Serial.print("URL : ");
        Serial.println(Send_Data_URL);

        //:::::::::::::::::: Envio de datos
        // Inicializadno HTTPClient como "http".
        HTTPClient http;

        // HTTP Obtener solicitud.
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        // HTTP codigo de estado.
        int httpCode = http.GET();
        Serial.print("HTTP code : ");
        Serial.println(httpCode);

        // Repuesta de Google Sheets
        String payload;
        if (httpCode > 0)
        {
            payload = http.getString();
            Serial.println("Envio satisfactorio...");
            // Serial.println("Payload : " + payload);
        }

        http.end();

        Serial.println("-------------");
    }
}

//---------------------------------------- Función para verificar los umbrales
void verifyThresholds()
{
    bool alarm = false;

    if (temperature > UMBRAL_TEMPERATURA)
    {
        Serial.println("¡Alerta! Temperatura excedida");
        alarm = true;
    }

    if (humidity > UMBRAL_HUMEDAD)
    {
        Serial.println("¡Alerta! Humedad excedida");
        alarm = true;
    }

    if (dioxidoCarbono > UMBRAL_CO2)
    {
        Serial.println("¡Alerta! Nivel de CO2 excedido");
        alarm = true;
    }

    if (alcohol > UMBRAL_ALCOHOL)
    {
        Serial.println("¡Alerta! Nivel de alcohol excedido");
        alarm = true;
    }

    if (intensidadLuz > UMBRAL_LUZ)
    {
        Serial.println("¡Alerta! Intensidad de luz excedida");
        alarm = true;
    }

    if (alarm)
    {
        digitalWrite(BUZZERPIN, HIGH);
    }
    else
    {
        digitalWrite(BUZZERPIN, LOW);
    }
}
