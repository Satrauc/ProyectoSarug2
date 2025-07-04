
BLECharacteristic* pTxCharacteristic;
BLECharacteristic* pRxCharacteristic;
bool deviceConnected = false;
String rxValue = "";

// Arreglos para almacenar valores
/*float Global[3];  // GEX, GEY, GEA
float Local[3];   // LEX, LEY, LEA
float PuntoB[2];  // BX, BY
float PuntoC[2];  // CX, CY
/**
void actualizarVariablesDesdeBLE() {
  GEX = Global[0];
  GEY = Global[1];
  GEA = Global[2];

  LEX = Local[0];
  LEY = Local[1];
  LEA = Local[2];

  BX = PuntoB[0];
  BY = PuntoB[1];

  CX = PuntoC[0];
  CY = PuntoC[1];
}
*/

/*void actualizarVariablesDesdeBLE() {
  GEX = Grobal[0];
  GEY = Grobal[1];
  GEA = Grobal[2];

  LEX = Local[0];
  LEY = Local[1];
  LEA = Local[2];

  BX = PuntoB[0];
  BY = PuntoB[1];

  CX = PuntoC[0];
  CY = PuntoC[1];
}*/
// ---------------------------------------------
// Funciónes: Para la configuracion del Bluetooth en ESP32
/*
  | Clase               |                                                     |
  | ------------------- | ---------------------------------------------------------------- |
  | `MyServerCallbacks` | Detecta conexiones y desconexiones BLE.                          |
  | `MyRxCallbacks`     | Reacciona cuando el cliente escribe datos en una característica. |

*/
// ---------------------------------------------

// Clase para eventos de conexión BLE
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
  }
  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    BLEDevice::startAdvertising();
  }
};

// Clase para recepción de datos desde MATLAB
class MyRxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) override {
    //rxValue = pCharacteristic->getValue();
    rxValue = String(pCharacteristic->getValue().c_str());
  }
};

// ---------------------------------------------
// Función: Configura inicial el Bluetooth del ESP32
// ---------------------------------------------
void setupBLE() {
  BLEDevice::init("ESP32_BLE");

  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Característica para enviar datos (notify)
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  // Característica para recibir datos (write)
  pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_RX,
    BLECharacteristic::PROPERTY_WRITE
  );
  pRxCharacteristic->setCallbacks(new MyRxCallbacks());

  pService->start();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("BLE iniciado");
}

void sendDataBLE(const String& msg) {
  if (deviceConnected) {
    Serial.println(msg);
    pTxCharacteristic->setValue(msg.c_str());
    pTxCharacteristic->notify();
  }
}

String readDataBLE() {
  return String(rxValue.c_str());
}

// ---------------------------------------------
// Función: Parsea la cadena de texto en arreglos
// Ejemplo de trama "GEX:10.5|GEY:20.2|GEA:30.3;LEX:5.0|LEY:15.0|LEA:25.0;BX:100.0|BY:200.0;CX:300.0|CY:400.0;";
// ---------------------------------------------
bool parsearDatos(String cadena) {
  bool respuesta = false;
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
        
        if (clave == "GEX") Global[0] = valor;
        else if (clave == "GEY") Global[1] = valor;
        else if (clave == "GEA") Global[2] = valor;
        else if (clave == "LEX") Local[0] = valor;
        else if (clave == "LEY") Local[1] = valor;
        else if (clave == "LEA") Local[2] = valor;
        else if (clave == "BX")  PuntoB[0] = valor;
        else if (clave == "BY")  PuntoB[1] = valor;
        else if (clave == "CX")  PuntoC[0] = valor;
        else if (clave == "CY")  PuntoC[1] = valor;
        respuesta = true;
      }
    }
  }
  return respuesta;
}

// ---------------------------------------------
// Función: Imprime los valores parseados
// ---------------------------------------------
void imprimirDatos() {
  Serial.println("Global:");
  Serial.println(Global[0]); Serial.println(Global[1]); Serial.println(Global[2]);

  Serial.println("Local:");
  Serial.println(Local[0]); Serial.println(Local[1]); Serial.println(Local[2]);

  Serial.println("Punto B:");
  Serial.println(PuntoB[0]); Serial.println(PuntoB[1]);

  Serial.println("Punto C:");
  Serial.println(PuntoC[0]); Serial.println(PuntoC[1]);
}
