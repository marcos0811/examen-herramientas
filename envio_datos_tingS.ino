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

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Conexión a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a ");
  Serial.print(ssid);
  Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conexión WiFi establecida");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Error: No se pudo conectar a la red WiFi.");
  }
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

  // Enviar los datos a ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String data = "api_key=" + apiKey + "&field1=" + String(temperatura) + "&field2=" + String(humedad);
    Serial.print("Enviando a ThingSpeak: ");
    Serial.println(data);
    
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(data);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Respuesta de ThingSpeak: ");
      Serial.println(response);
    } else {
      Serial.print("Error al enviar a ThingSpeak. Código de error: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("Error: WiFi no conectado. No se pueden enviar datos a ThingSpeak.");
  }

  delay(15000);  // Espera de 20 segundos entre cada envío de datos
}
