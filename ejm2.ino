#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Definición del tipo de sensor DHT
#define DHT_PIN 22
#define DHT_TYPE DHT11

// Configuración de WiFi
const char* ssid = "examen_herramientas";           // Reemplaza con tu SSID
const char* password = "1450329923"; // Reemplaza con tu contraseña

// Configuración de ThingSpeak
const String apiKey = "1PFTU3FYLRD3KO9F";  // Reemplaza con tu clave API
const String apiUrl = "http://api.thingspeak.com/update";

// Inicialización del sensor DHT
DHT dht(DHT_PIN, DHT_TYPE);

// Configuración del LED
#define LED_PIN 13

// Configuración del puerto serial
#define BAUD_RATE 115200
#define SERIAL_TX 17
#define SERIAL_RX 16

unsigned long previousMillisLabVIEW = 0; // Tiempo anterior para LabVIEW
unsigned long previousMillisThingSpeak = 0; // Tiempo anterior para ThingSpeak
const long intervalLabVIEW = 2000; // Intervalo para LabVIEW (2 segundos)
const long intervalThingSpeak = 15000; // Intervalo para ThingSpeak (15 segundos)

void setup() {
  Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_TX, SERIAL_RX);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);

  // Conexión a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
}

void loop() {
  unsigned long currentMillis = millis();

  // Lectura de la humedad y temperatura desde el sensor DHT11
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Verificación de lecturas válidas
  if (!isnan(humedad) && !isnan(temperatura)) {
    // Enviar los datos a LabVIEW cada 2 segundos
    if (currentMillis - previousMillisLabVIEW >= intervalLabVIEW) {
      previousMillisLabVIEW = currentMillis;
      Serial.print(temperatura);
      Serial.print(",");
      Serial.println(humedad);
    }

    // Enviar los datos a ThingSpeak cada 15 segundos
    if (currentMillis - previousMillisThingSpeak >= intervalThingSpeak) {
      previousMillisThingSpeak = currentMillis;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String data = "api_key=" + apiKey + "&field1=" + String(temperatura) + "&field2=" + String(humedad);
        http.begin(apiUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int httpResponseCode = http.POST(data);
        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("Datos enviados a ThingSpeak: " + response);
        } else {
          Serial.println("Error enviando los datos a ThingSpeak");
        }
        http.end();
      }
    }
  } else {
    Serial.println("Error al leer el sensor DHT11.");
  }

  // Verificar si hay datos disponibles en el puerto serial
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();

    // Encender o apagar el LED según el carácter recibido
    if (receivedChar == 'a') {
      digitalWrite(LED_PIN, HIGH); // Encender el LED
    } else if (receivedChar == 'b') {
      digitalWrite(LED_PIN, LOW); // Apagar el LED
    }
  }
}
