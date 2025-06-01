
void configuracionLeds() {
  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
}

// Funciones para controlar los LEDs
void encenderVerde() {
  digitalWrite(GreenLed, HIGH);
}

void apagarVerde() {
  digitalWrite(GreenLed, LOW);
}

void encenderRojo() {
  digitalWrite(RedLed, HIGH);
}

void apagarRojo() {
  digitalWrite(RedLed, LOW);
}