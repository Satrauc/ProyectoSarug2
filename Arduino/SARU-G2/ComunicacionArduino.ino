#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Arreglos para almacenar valores
float Grobal[3];  // GEX, GEY, GEA
float Local[3];   // LEX, LEY, LEA
float PuntoB[2];  // BX, BY
float PuntoC[2];  // CX, CY

void config(){
   SerialBT.begin("ESP32_Chat");
   Serial.println("Bluetooth iniciado. Esperando conexión...");
}

void parsearDatos(String cadena) {
  // Divide por secciones: ";"
  int i = 0;
  while (cadena.length() > 0) {
    int index = cadena.indexOf(';');
    String segmento = (index != -1) ? cadena.substring(0, index) : cadena;
    if (index != -1) cadena = cadena.substring(index + 1);
    else cadena = "";

    // Divide por campos: "|"
    int j = 0;
    while (segmento.length() > 0) {
      int sep = segmento.indexOf('|');
      String campo = (sep != -1) ? segmento.substring(0, sep) : segmento;
      if (sep != -1) segmento = segmento.substring(sep + 1);
      else segmento = "";

      // Divide por clave:valor
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
