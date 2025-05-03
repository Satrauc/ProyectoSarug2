#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Arreglos para almacenar valores
float Grobal[3];  // GEX, GEY, GEA
float Local[3];   // LEX, LEY, LEA
float PuntoB[2];  // BX, BY
float PuntoC[2];  // CX, CY

// ---------------------------------------------
// Función: Configura el Bluetooth del ESP32
// ---------------------------------------------
void configurarBluetooth() {
  Serial.begin(115200);
  delay(500); // Tiempo para estabilizar
  SerialBT.begin("ESP32_Chat"); // Nombre visible del Bluetooth
  Serial.println("Bluetooth iniciado. Esperando conexión...");
}

// ---------------------------------------------
// Función: Recibe la cadena y responde
// ---------------------------------------------
void recibirYResponder() {
  if (SerialBT.available()) {
    String recibido = SerialBT.readStringUntil('\n');
    Serial.println("Cadena recibida:");
    Serial.println(recibido);

    parsearDatos(recibido);

    // Mostrar los datos parseados por consola
    imprimirDatos();

    SerialBT.println("Datos recibidos y parseados correctamente.");
  }
}

// ---------------------------------------------
// Función: Parsea la cadena de texto en arreglos
// ---------------------------------------------
void parsearDatos(String cadena) {
  while (cadena.length() > 0) {
    int index = cadena.indexOf(';');
    String segmento = (index != -1) ? cadena.substring(0, index) : cadena;
    cadena = (index != -1) ? cadena.substring(index + 1) : "";

    while (segmento.length() > 0) {
      int sep = segmento.indexOf('|');
      String campo = (sep != -1) ? segmento.substring(0, sep) : segmento;
      segmento = (sep != -1) ? segmento.substring(sep + 1) : "";

      int dosPuntos = campo.indexOf(':');
      if (dosPuntos != -1) {
        String clave = campo.substring(0, dosPuntos);
        float valor = campo.substring(dosPuntos + 1).toFloat();

        if (clave == "GEX") Grobal[0] = valor;
        else if (clave == "GEY") Grobal[1] = valor;
        else if (clave == "GEA") Grobal[2] = valor;
        else if (clave == "LEX") Local[0] = valor;
        else if (clave == "LEY") Local[1] = valor;
        else if (clave == "LEA") Local[2] = valor;
        else if (clave == "BX")  PuntoB[0] = valor;
        else if (clave == "BY")  PuntoB[1] = valor;
        else if (clave == "CX")  PuntoC[0] = valor;
        else if (clave == "CY")  PuntoC[1] = valor;
      }
    }
  }
}
// ---------------------------------------------
// Función: Imprime los valores parseados
// ---------------------------------------------
void imprimirDatos() {
  Serial.println("Grobal:");
  Serial.println(Grobal[0]); Serial.println(Grobal[1]); Serial.println(Grobal[2]);

  Serial.println("Local:");
  Serial.println(Local[0]); Serial.println(Local[1]); Serial.println(Local[2]);

  Serial.println("Punto B:");
  Serial.println(PuntoB[0]); Serial.println(PuntoB[1]);

  Serial.println("Punto C:");
  Serial.println(PuntoC[0]); Serial.println(PuntoC[1]);
}
