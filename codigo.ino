#include <DHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Definición del tipo de sensor DHT
#define DHT_PIN 22
#define DHT_TYPE DHT11

// Configuración del puerto serial
#define BAUD_RATE 115200
#define SERIAL_TX 17
#define SERIAL_RX 16

// Inicialización del sensor DHT
DHT dht(DHT_PIN, DHT_TYPE);

// Configuración del LED
#define LED_PIN 13

// Configuración de WiFi
const char* ssid = "examen_herramientas";
const char* password = "1450329923";

// Configuración de ThingSpeak
const char* thingspeak_server = "api.thingspeak.com";
const String api_key = "1PFTU3FYLRD3KO9F";

WiFiClientSecure client;

void setup() {
  Serial.begin(BAUD_RATE);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  
  // Conexión a WiFi
  connectWiFi();

  // Apagar el LED al inicio
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Lectura de la humedad y temperatura desde el sensor DHT11
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Verificación de lecturas válidas
  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT11.");
    return;
  }

  // Enviar los datos separados por comas por el puerto serial
  Serial.print(temperatura);
  Serial.print(",");
  Serial.println(humedad);

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

  // Enviar datos a ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    sendToThingSpeak(temperatura, humedad);
  }

  delay(2000);  // Espera de 2 segundos entre cada lectura y envío a ThingSpeak
}

void connectWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conexión WiFi establecida.");
}

void sendToThingSpeak(float temp, float hum) {
  String url = "/update?api_key=" + api_key + "&field1=" + String(temp) + "&field2=" + String(hum);
  
  if (!client.connect(thingspeak_server, 443)) {
    Serial.println("Error al conectarse a ThingSpeak.");
    return;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + thingspeak_server + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Datos enviados a ThingSpeak:");
  Serial.println("Temperatura: " + String(temp) + "°C");
  Serial.println("Humedad: " + String(hum) + "%");

  client.stop();
}
