void setup() {
    Serial.begin(9600);
}

void loop() {
    //funcion_modular();  // Llamada a la función que está en otro archivo
    delay(1000);
}
/******************************************************************************/
//Ejemplo QRE1114
/******************************************************************************/
/**
const int sensores[] = {A0, A1, A2};  // Usa de 1 a 4 sensores
const int cantidadSensores = 3;
const int umbral = 500;

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
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Arreglos para almacenar los valores
float Grobal[3];   // GEX, GEY, GEA
float Local[3];    // LEX, LEY, LEA
float PuntoB[2];   // BX, BY
float PuntoC[2];   // CX, CY

void setup() {
  configurarBluetooth();
}

void loop() {
  recibirYResponder();
}
/**/
/**/
/******************************************************************************/
//Ejemplo MPU6500
/******************************************************************************/
/**
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <vector>

MPU9250_asukiaaa MPU6500;

// Variables para posición y ángulo
float posX = 0, posY = 0;
float velX = 0, velY = 0;
float angleZ = 0;
unsigned long lastTimePos = 0;
bool isFirstPosCall = true;

void setup() {
  Serial.begin(115200);
  ConfigMpu6500();
  delay(1000); // Espera para que el sensor esté listo
}

void loop() {
  
  UpdateMPU6500(); // Obtener nuevos datos del sensor

  // Obtener datos de posición y ángulo
  std::vector<float> pose = GetPositionAndAngle();
  // Obtener velocidad independiente
  std::vector<float> velocity = GetCurrentVelocity();

  // Mostrar resultados por Serial
  Serial.print("Posición -> X: "); Serial.print(pose[0], 2);
  Serial.print(" | Y: "); Serial.print(pose[1], 2);
  Serial.print(" | Ángulo Z: "); Serial.print(pose[2], 2);
  
  Serial.print(" || Velocidad -> Vx: "); Serial.print(velocity[0], 2);
  Serial.print(" | Vy: "); Serial.println(velocity[1], 2);

  delay(100); // Espera entre lecturas
}
/**/
