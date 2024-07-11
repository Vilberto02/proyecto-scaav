//---------------------------------------- Metadata del dispositivo en Blynk
#define BLYNK_TEMPLATE_ID "*******"
#define BLYNK_TEMPLATE_NAME "******"
#define BLYNK_AUTH_TOKEN "********"

//---------------------------------------- Librerias
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>
#include <Wire.h>
#include <BH1750.h>
#include <string.h>

//---------------------------------------- Credenciales para la conexión a internet
char ssid[] = "**********";
char pass[] = "**********";

//---------------------------------------- Pines de los sensores
#define DHTPIN 4
#define MQ135PIN 32
#define MQ3PIN 33
#define BUZZERPIN 2
#define BUZZER2PIN 26
#define PIRPIN 27

//---------------------------------------- Inicializacion de objetos
DHTesp dht;
BH1750 luxometro;

//---------------------------------------- Variables de los sensores
float temperature;
int humidity;
float alcohol;
float BAC = 0.0;
double dioxidoCarbono;
float intensidadLuz;
double CO2ppm = 0.0;
bool movimiento = false;
const int frecuenciaBuzzer = 1000;

//---------------------------------------- Umbrales
const float UMBRAL_TEMPERATURA_MAX = 20.0;
const float UMBRAL_TEMPERATURA_MIN = 13.0;
const int UMBRAL_HUMEDAD_MAX = 90;
const int UMBRAL_HUMEDAD_MIN = 85;
const float UMBRAL_CO2_MAX = 700.0;
const float UMBRAL_CO2_MIN = 550.0;
const float UMBRAL_ALCOHOL_MAX = 500.0;
const float UMBRAL_ALCOHOL_MIN = 300.0;
const float UMBRAL_LUZ_MAX = 50.0;
const float UMBRAL_LUZ_MIN = 0.0;

//---------------------------------------- Variables para el envio de datos a Google Sheet
String Status_Read_Sensor = "";
String Web_App_URL = "https://script.google.com/macros/s/AKfycbwYIZUlWYsuEnmP7FWZCTxik8ihst91UsA7i6eAtZ-pnmrGhhZO2GEYq97UIuDCHJq_/exec"; // Script de Google

// Timer de Blynk
BlynkTimer timer;

//---------------------------------------- Prototipo de las funciones
void sendAllToBlynk();
void calibrateMQ3();
void calibrateMQ135();
void sendDataGoogleSheet();
void verifyThresholds();
// void detectMotion();

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
        Status_Read_Sensor = "Success";
    }

    Serial.print("\n\nTemperatura: ");
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
    // calibrateMQ135();
    dioxidoCarbono = analogRead(MQ135PIN);
    // dioxidoCarbono = CO2ppm;
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
    // calibrateMQ3();
    alcohol = analogRead(MQ3PIN);
    // alcohol = BAC;
    Serial.print("Nivel de alcohol: ");
    Serial.print(alcohol);
    Serial.println(" ppm");
    Blynk.virtualWrite(V3, alcohol);
}

void calibrateMQ3()
{
    /*
    PRIMERA PARTE DE LA CALIBRACION PARA HALLAR RO
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
    Serial.println(R0);

    // RO obteniendo a partir de las pruebas. RO = 0.02
    */

    // SEGUNDA PARTE
    float sensor_volt;
    float RS_gas;    // Get value of RS in a GAS
    float ratio;     // Get ratio RS_GAS/RS_air
    float R0 = 0.02; // Es una constante a partir de las pruebas hechas
    int sensorValue = analogRead(MQ3PIN);
    sensor_volt = (float)sensorValue / 1024 * 3.0; // 3 voltios
    RS_gas = (3.0 - sensor_volt) / sensor_volt;    // omit *RL (3 voltios)

    /* -Replace the name "R0" with the value of R0 in the demo of First Test - */
    ratio = RS_gas / R0; // ratio = RS/R0
    // BAC = 0.1896 * ratio ^ 2.0 - 8.6178 * ratio / 10 + 1.0792; // BAC in mg/L
    BAC = 0.1896 * pow(ratio, 2.0) - 8.6178 * ratio / 10 + 1.0792; // BAC in mg/L
    Serial.print("BAC = ");
    Serial.println(BAC * 0.0001); // convert to g/dL
    // Serial.print("\n\n");

    //  delay(1000);
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

    pinMode(BUZZERPIN, OUTPUT);
    // pinMode(BUZZER2PIN, OUTPUT);
    digitalWrite(BUZZERPIN, LOW);
    // digitalWrite(BUZZER2PIN, LOW);
    pinMode(PIRPIN, INPUT);
    luxometro.begin();
    Serial.println("Sensor BH175O inicializado");
    dht.setup(DHTPIN, DHTesp::DHT11);
    Serial.println("Sensor DHT11 inicializado");
    // Setup a function to be called every second
    timer.setInterval(1000L, sendAllToBlynk);
    timer.setInterval(120000L, sendDataGoogleSheet); // Enviar datos a Google Sheet cada 2 minutos
    timer.setInterval(1000L, verifyThresholds);      // Verificar umbrales cada segundo
    // timer.setInterval(1000L, detectMotion);
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
        /*Agregado al sheet anterior pero en una nueva hoja*/
        Send_Data_URL += "&motion=" + String(movimiento);

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

    if (temperature > UMBRAL_TEMPERATURA_MAX) // Las frutas necesitan de una temperatura baja.
    {
        Serial.println("¡Alerta! Temperatura excedida");
        alarm = true;
    }

    if (humidity < UMBRAL_HUMEDAD_MIN) // Las frutas necesitan estar en un ambiente de alta humedad
    {
        Serial.println("¡Alerta! baja humedad");
        alarm = true;
    }

    if (dioxidoCarbono > UMBRAL_CO2_MAX) // No debe de sobrepasar el umbral maximo de CO2
    {
        Serial.println("¡Alerta! Nivel de CO2 excedido");
        alarm = true;
    }

    if (alcohol > UMBRAL_ALCOHOL_MAX) // No debe de sobrepasar el umbral maximo de alcohol
    {
        Serial.println("¡Alerta! Nivel de alcohol excedido");
        alarm = true;
    }

    if (intensidadLuz > UMBRAL_LUZ_MAX) // No debe de sobrepasar el umbral maximo de intensidad de luz
    {
        Serial.println("¡Alerta! Intensidad de luz excedida");
        alarm = true;
    }

    if (alarm)
    {
        tone(BUZZER2PIN, frecuenciaBuzzer);
    }
    else
    {
        noTone(BUZZER2PIN);
    }
}

//---------------------------------------- Función para el sensor de movimiento PIR
/*void detectMotion()
{
    movimiento = digitalRead(PIRPIN);
    Serial.print("Valor PIR: ");
    Serial.println(movimiento);
    if (movimiento)
    {
        tone(BUZZER2PIN, frecuenciaBuzzer);
        Serial.println("Movimiento detectado!");
    }
    else
    {
        noTone(BUZZER2PIN);
        Serial.println("No hay movimiento");
    }
}*/
