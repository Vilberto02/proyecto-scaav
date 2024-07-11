//---------------------------------------- Librerias
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <DHTesp.h>
#include <BH1750.h>

//---------------------------------------- objetos de las clases
DHTesp dht;
BH1750 Luxometro;

//---------------------------------------- Pines de los sensores
#define DHTPIN 4
#define MQ3PIN 33
#define MQ135PIN 32

//---------------------------------------- Conexión al Wifi
const char *ssid = "********";
const char *password = "*******";

//---------------------------------------- Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbzVJ1kzCxN0LpAfc1YpUmH8JLgWs_K4vQ7laYJWxOgJ2ZAnyDcvg-8XEWp5qVUepCp5/exec";

//---------------------------------------- Variables
float dioxidoCarbono;
float alcohol;
float intensidadLuz;
String Status_Read_Sensor = "";
float temperatura;
int humedad;

//---------------------------------------- Captura de los datos del sensor de temperatura y humedad DHT11
void sensorDHT11()
{
    humedad = dht.getHumidity();
    temperatura = dht.getTemperature();

    // Verificar si el sensor esta captando correctamente los valores.
    if (isnan(humedad) || isnan(temperatura))
    {
        Serial.println();
        Serial.println(F("Error en la lectura del sensor DHT11"));
        Serial.println();

        Status_Read_Sensor = "Failed";
        temperatura = 0.00;
        humedad = 0;
    }
    else
    {
        Status_Read_Sensor = "Success";
    }

    Serial.println();
    Serial.println("-------------");
    Serial.print(F("Estado del sensor DHT11: "));
    Serial.print(Status_Read_Sensor);
    Serial.print(F(" | Humidity : "));
    Serial.print(humedad);
    Serial.print(F("% | Temperature : "));
    Serial.print(temperatura);
    Serial.print(F("°C"));
    Serial.println("-------------");
}

//---------------------------------------- Captura de los datos del sensor de alcohol MQ3
void sensorMQ3()
{
    alcohol = analogRead(MQ3PIN);
    Serial.println();
    Serial.println("-------------");
    Serial.print(F("Nivel de alcohol: "));
    Serial.println(alcohol);
    Serial.println(" ppm");
    Serial.println("-------------");
}

//---------------------------------------- Captura de los datos del sensor de CO2 MQ135
void sensorMQ135()
{
    dioxidoCarbono = analogRead(MQ135PIN);
    Serial.println();
    Serial.println("-------------");
    Serial.print(F("Nivel de CO2: "));
    Serial.print(dioxidoCarbono);
    Serial.println(" ppm");
    Serial.println("-------------");
}

//---------------------------------------- Captura de los datos del sensor de intensidad de luz BH1750
void sensorBH1750()
{
    intensidadLuz = Luxometro.readLightLevel();
    Serial.println();
    Serial.println("-------------");
    Serial.print(F("Intensidad de luz: "));
    Serial.print(intensidadLuz);
    Serial.println(" lx");
    Serial.println("-------------");
}

//---------------------------------------- Funcion SETUP
void setup()
{
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    dht.setup(DHTPIN, DHTesp::DHT11);
    Serial.println("DHT inicializado");
    Luxometro.begin();
    Serial.println("BH1750 inicializado");
    delay(1000);

    //----------------------------------------Wifi modo STA
    Serial.println();
    Serial.println("-------------");
    Serial.println("WIFI mode : STA");
    WiFi.mode(WIFI_STA);
    Serial.println("-------------");

    //----------------------------------------Conectado a Wi-Fi (STA).
    Serial.println();
    Serial.println("------------");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    //:::::::::::::::::: Proceso de conexión del ESP32 con WiFi Hotspot / WiFi Router
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(250);
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
    Serial.println("------------");
}

//---------------------------------------- Funcion LOOP
void loop()
{
    // Sensores
    sensorDHT11();
    sensorMQ3();
    sensorMQ135();
    sensorBH1750();

    //---------------------------------------- Verificar si el ESP32 esta conectado al Wifi
    // Enviar datos de los sensores a Google Sheets.
    if (WiFi.status() == WL_CONNECTED)
    {
        String Send_Data_URL = Web_App_URL + "?sts=write";
        Send_Data_URL += "&srs=" + Status_Read_Sensor;
        Send_Data_URL += "&temp=" + String(temperatura);
        Send_Data_URL += "&humd=" + String(humedad);
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
            Serial.println("Payload : " + payload);
        }

        http.end();

        Serial.println("-------------");
    }

    delay(180000); // Cada 3 minutos
}