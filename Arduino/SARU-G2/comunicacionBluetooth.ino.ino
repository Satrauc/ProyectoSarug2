#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  delay(500); // Espera antes de imprimir
  SerialBT.begin("ESP32_Chat"); 
  Serial.println("Bluetooth iniciado. Esperando conexión...");
}

void loop() {
  if (SerialBT.available()) {
    String recibido = SerialBT.readStringUntil('\n');
    Serial.print("Mensaje recibido desde MATLAB: ");
    Serial.println(recibido);

    SerialBT.println("Mensaje recibido: " + recibido);
  }
}



