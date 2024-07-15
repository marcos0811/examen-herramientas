#include <DHT.h>

// Definimos el tipo de sensor DHT
#define DHT_PIN 22
#define DHT_TYPE DHT11

// Inicializamos el sensor DHT
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  // valores de la humedad y temperatura desde el sensor DHT11
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Verificamos si las lecturas son válidas
  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT11.");
    return;
  }

  // Enviamos los datos separados por comas por el puerto serial al labview
  Serial.print(temperatura);
  Serial.print(",");
  Serial.println(humedad);

  delay(2000);  // Esperamos de 2 segundos entre cada lectura
}
