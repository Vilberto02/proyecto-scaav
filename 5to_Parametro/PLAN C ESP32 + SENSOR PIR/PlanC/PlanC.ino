#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Reemplazar con los datos de tu red WIFI
const char* ssid = "HUAWEI P smart 2019";
const char* password = "sebitas1";

// Inicializar BOT Telegram
#define BOTtoken "xxxxxxxxxxxxxxx"  // Tu Bot Token (Obtener de Botfather)

// Usa @myidbot para averiguar el chat ID
// También necesita hacer clic en "start" antes de enviarle mensajes al bot
#define CHAT_ID "xxxxxxxxxxxxx"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int buzPin = 25;
const int motionSensor = 27; // Sensor PIR
bool motionDetected = false;
bool alarmActivated = false;  // Cambiado a false para que la alarma esté desactivada desde el principio

const int potPin = 34; // Pin analógico donde está conectado el potenciómetro
int potValue = 0; // Valor del potenciómetro
int lastPotValue = 0; // Valor anterior del potenciómetro

void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Usuario No Autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/alarma_on") {
      bot.sendMessage(chat_id, "Alarma activada", "");
      alarmActivated = true;
    }

    if (text == "/alarma_off") {
      bot.sendMessage(chat_id, "Alarma desactivada", "");
      alarmActivated = false;
      digitalWrite(buzPin, LOW);
    }
  }
}

void updatePotValue() {
  // Lee el valor actual del potenciómetro
  int rawValue = analogRead(potPin);

  // Enviar mensaje a Telegram solo si el valor del potenciómetro ha cambiado significativamente
  if (abs(rawValue - lastPotValue) > 10) {
    potValue = map(rawValue, 0, 4095, 100, 2000);  // Ajusta el rango según sea necesario
    lastPotValue = rawValue;

    // Enviar mensaje a Telegram con la nueva frecuencia
    bot.sendMessage(CHAT_ID, "Frecuencia ajustada: " + String(potValue) + " Hz", "");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(motionSensor, INPUT_PULLUP);
  pinMode(buzPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Al inicio, la alarma estará desactivada
  bot.sendMessage(CHAT_ID, "Bot iniciado. Alarma desactivada.", "");
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  if (motionDetected && alarmActivated) {
    bot.sendMessage(CHAT_ID, "Movimiento detectado!!", "");
    motionDetected = false;
    tone(buzPin, 1000);  // Puedes ajustar la frecuencia según tus preferencias
    delay(5000);
    noTone(buzPin);
  }

  updatePotValue();  // Actualiza el valor del potenciómetro
}
