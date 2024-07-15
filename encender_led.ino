// Definimos el pin del led
#define LED_PIN 13

void setup() {
  Serial.begin(115200); 
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
    char val = Serial.read(); //leemos los datos enviados del labview
    if (val == 'a') {
      digitalWrite(LED_PIN, HIGH);  // Encendemos el led
    } else if (val == 'b') {
      digitalWrite(LED_PIN, LOW);   // Apagamos el led
  }
}
