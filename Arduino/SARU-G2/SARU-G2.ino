//COMUNICACION
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

  // UUIDs personalizados
  #define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
  #define CHARACTERISTIC_TX   "abcd1234-ab12-cd34-ef56-abcdef123456"  // ESP32 → MATLAB
  #define CHARACTERISTIC_RX   "dcba4321-ba21-dc43-fe65-fedcba654321"  // MATLAB → ESP32

//VELOCIDAD Y POCISION
#include "FastIMU.h"
#include <Wire.h>
#include <vector>

  #define IMU_ADDRESS 0x68
  MPU6500 IMU;

//MATRIZ TRANSFORMACIÓN
#include <math.h>

// LEDS

  #define RedLed 18
  #define GreenLed 19



void setup() {
    Serial.begin(115200);
}

void loop() {
    
    delay(1000);
}
/******************************************************************************/
//Ejemplo QRE1114
/******************************************************************************/
/**
void setup() {
  Serial.begin(9600);
  configurarSensores(sensores, cantidadSensores);
}

void loop() {
  bool objeto = detectarObjetoRedundante(umbral, sensores, cantidadSensores);
  Serial.println(objeto ? "Objeto detectado" : "Sin objeto");
  delay(200);
}
/**/
/******************************************************************************/
//Ejemplo Comunicación
/******************************************************************************/
/**
void setup() {
  Serial.begin(115200);
  setupBLE();
}

void loop() {
  String Data = readDataBLE();
  if(Data!=""){
    parsearDatos(Data);
    Serial.println(Data);
    imprimirDatos() ;
  }
  //sendDataBLE("Hola desde ESP32");
  delay(1000);
}

/**/
/**/
/******************************************************************************/
//Ejemplo MPU6500
/******************************************************************************/
/**

/**/
//*****************************************************************
// Ejemplo Motores
//*****************************************************************
/**
void setup() {
  Serial.begin(115200);

  ConfigMotor();
  ConfigEncoder(330, 6.2); // Encoder turncount , diameter
}

void loop() {

  //LinealMotor(50, 115);
  AngularMotor(90, 255);
  //RmpDistance();
  delay(10);
}
/**/
