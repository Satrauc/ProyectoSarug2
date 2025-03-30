void setup() {
    Serial.begin(9600);
}

void loop() {
    funcion_modular();  // Llamada a la función que está en otro archivo
    delay(1000);
}
