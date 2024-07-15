//Inculimos las librerias necesarias para este proyecto
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Pin analogico al que esta conectado el sensor DHT11
#define DHT_PIN 22 
// Pin analogico del led
#define LED_PIN 13
#define DHT_TYPE DHT11

// Configuración de la red wiFi
const char* ssid = "examen_herramientas";  // se crea una red de la computador para el dia de la exposcion no tener inconvenientes con la red
const char* password = "1450329923"; // contraseña de la red

// Configuramos de thingSpeak
const String apiKey = "1PFTU3FYLRD3KO9F";  // apikey para escribir los datos en el thinkspeak
const String apiUrl = "http://api.thingspeak.com/update"; // url del thingspeak

// Inicialiamos del sensor DHT
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillisLabVIEW = 0; // Tiempo anterior para LabVIEW
unsigned long previousMillisThingSpeak = 0; // Tiempo anterior para ThingSpeak
const long intervalLabVIEW = 1000; // Intervalo para el envio de datos al labview de 1 segundo
const long intervalThingSpeak = 15000; // Intervalo para el envio de datos al thingspeak 15 segundos ya que tenemos la version gratuita

void setup() {
  Serial.begin(115200); //iniciamos el puerto serial con un valor de 115200 baudios para la comunicacion
  dht.begin(); // se inicia el sensor dht11
  pinMode(LED_PIN, OUTPUT);   //se configura el pin del led como una salida

  // Conexión a WiFi
  WiFi.begin(ssid, password); //Se establece la conexion con la red wifi
  while (WiFi.status() != WL_CONNECTED) { //este while se ejecutara hasta que se conecte con la red wifi
    delay(1000); //esperamos un segundo antes de volver a intentar de reconectar de nuevo con la red
  }
}

void loop() {
  unsigned long currentMillis = millis(); //optenemos el tiempo den milisegundos desde que se inicio a ejecutar, para que de esta froma enviar los datos al labview y al thingspeak correctamente
  float humedad = dht.readHumidity(); // se lee el valor de la humedad del dht11
  float temperatura = dht.readTemperature(); // se lee el valor de la temperatura del dht11
  
  // Verificación que las lecturas sean validas con el comando isnan para verificar que tenga valores y no envie datos vacios
  if (!isnan(humedad) && !isnan(temperatura)) {
    
    if (currentMillis - previousMillisLabVIEW >= intervalLabVIEW) { //se verifica si paso 1 segundo para enviar al labview los datos
      previousMillisLabVIEW = currentMillis; // actualizamos el valor anterior del evnio de datos al labvidew
      Serial.print(temperatura); //se envia el valor de temperatura al labview
      Serial.print(","); //establecemos un caracter que se usara en el labview para la filtracion de los datos
      Serial.println(humedad); // se envia el valor de la humedad
    }

    if (currentMillis - previousMillisThingSpeak >= intervalThingSpeak) { // se verifica que pasaron 15 segundos antes de enviar la lectura de los valores
      previousMillisThingSpeak = currentMillis; // se acutaliza el valor anterior del envio de datos al thingspeak
      
      if (WiFi.status() == WL_CONNECTED) { // verificamos tener conexion a la ed para proseguir con el envio
        HTTPClient http; // generamos una instancia de calse http para manejar el envio de datos al servidor thingspeak
        
        int ledState = digitalRead(LED_PIN); // leemos el estado del led para la graficacion

        // Construimos la cadena de texto(data) para el envio de datos al thingspeak con los valores de la temperatura,humedad y el led(1-encendido o 0-apagado) 
        String data = "api_key=" + apiKey + "&field1=" + String(temperatura) + "&field2=" + String(humedad) + "&field3=" + String(ledState);
        http.begin(apiUrl); // se inicia una solicitud http para enviar los datos al thingspeak
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //establecemos el tipo de contenido de la solicitud, el que se puede ver es el fromato estandar para enviar datos en formularios http
        int httpResponseCode = http.POST(data); //aqui se envin los datos al servidor con el formato data creado anteriormente (data)
        
        http.end(); // finaliza la sesoin http y libera los recursos utilizados
      }
    }
  }

  char receivedChar = Serial.read(); // Leemos el carácter recibido por serial para controlar el LED el cual es el valor maximo de la temperatura
  if (receivedChar == 'a') { // Si el valor de temp subera a un valor ya establecido envia un string 'a'
    digitalWrite(LED_PIN, HIGH); //esto significa que la temperatura es mayor a un valor ya establecido
  } else if (receivedChar == 'b') { // si no supera ningun valor envia un string 'b'
    digitalWrite(LED_PIN, LOW); // se mantiene el eld apagado o se le apaga en el caso que se encendio
  }
}
