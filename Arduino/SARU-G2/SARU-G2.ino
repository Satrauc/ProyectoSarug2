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
  #define N 10 // Para filtrar
  #define N_CAL 100
  #define UMBRAL_ACEL 10.0   // cm/s²
  #define UMBRAL_VEL 0.0    // cm/s
  #define TIEMPO_QUIETO_MS 200
  #define IMU_ADDRESS 0x69
  MPU6500 IMU;

//MATRIZ TRANSFORMACIÓN
#include <math.h>

// LEDS
  #define RedLed 18
  #define GreenLed 19

// DETECION PLANTA
  #define QRE_PIN 36 
  std::vector<float> obtenerPosicionXY();

/***************************************************************/
// Variables Main SARU
/***************************************************************/
bool Enable = false;        // Verifica el estado del robot Saru-G2
std::vector<float> PocisionActual = {0.0, 0.0};
float AnguloActual = 90.0;
float VelocidadActual = 0.0;
float Rpms = 0.0;
float distanciaAB, anguloAB, anguloBA;
float distanciaBC, anguloBC, anguloCB, anguloA;
unsigned char Estado = 0;    // En Punto A = 0
                             // Hacia Punto B = 1
                             // Hacia Punto C = 2
                             // Espera remove planta = 3
                             // Hacia Punto B desde C = 4
                             // Hacia Punto A desde B = 5
/**/
void setup() {
    Serial.begin(115200);

    // Inicialización de hardware y módulos
    setupBLE();
    configurarSensores();
    configuracionLeds();
    ConfigMotor();
    ConfigEncoder(0,0.0);
    configurarMPU6500();
    encenderRojo(); 
    
    // Proceso leds
    // 1- Inicia con led rojo en espera de los datos de matlab
    // 2- Parpadea en verde esperando planta
    // 3- led rojo apagado y verde activo en proceso de A a B y a C
    // 4- Parpadea rojo hasta retirar la planta 
    // 5- led rojo apagado y verde activo en proceso de C a B y a A
    // 6- Vuelve a led rojo en espera de datos de matlab
    
}

void loop() {
    
  if(Enable){// Si el robot esta conextado y activo 
    // Ya hay datos en los arreglos
    /**/
    //************************************
    // 2. ¿Planta ubicada correctamente?
    //************************************
    while (!detectarObjetoRedundante()) {
      int espera = 500;
      encenderVerde(); //Parpadeo de led verde en espera de planta
      delay(espera/2);
      apagarVerde();
      delay(espera/2);
    }
    encenderVerde();
    
    
    // Envio de datos del robot 
    actualizarValores();
    UpdateEncoderA();
    UpdateEncoderB();

  
    PocisionActual =  obtenerPosicionXY();
    VelocidadActual = obtenerVelocidadX();
    AnguloActual = obtenerAnguloZ();
    Rpms = RmpDistance();

    //************************************
    // 3.2. Envio de datos a matlab
    //************************************
    sendDataBLE("PX:" + String(PocisionActual[0], 3)+"|PY:" + String(PocisionActual[1], 3)+"|V:" + String(VelocidadActual, 3)+"|A:"+ String(AnguloActual, 3)+"|R:"+String(Rpms, 3));
    //actualizarVariablesDesdeBLE();
    switch (Estado) {
      case 0:
        // En Punto A 

        
      //************************************
      // 3.1. Calculo de trayectorias
      //************************************
        calcularTrayectoria();
        distanciaAB = getDistancia_AB();
        anguloAB = normalizarAngulo(getAngulo_AB());
        distanciaBC = getDistancia_BC();
        anguloBC = normalizarAngulo(getAngulo_BC());
        Estado = 1; 

        break;

      case 1:
        // Hacia Punto B

        Girar(anguloAB);
        Mover(distanciaAB, false);
        delay(10000);
        Estado = 2; 
        break;

      case 2:
        // Hacia Punto C 
        Girar(anguloBC);
        Mover(distanciaBC, false);
        Estado = 3; 
        break;

      case 3:
        //************************************
        // 4. Espera remove planta
        //************************************
        while (detectarObjetoRedundante()) {
          int espera = 500;
          encenderRojo();//Parpadeo de led rojo  en espera de retiro de planta
          delay(espera/2);
          apagarRojo();
          delay(espera/2);
        }
        //************************************
        // 4. Calculo de trayectoria de regreso
        //************************************
        volverA_A();
        anguloBA = normalizarAngulo(getAngulo_BA());
        anguloCB = normalizarAngulo(getAngulo_CB());
        anguloA = normalizarAngulo(getAngulo_A());

        Estado = 4; 
        break;
      
      case 4:
        // Hacia Punto B desde C
        Girar(anguloCB);
        Mover(distanciaBC, false);
          Estado = 5; 
        break;
      
      case 5:
        // Hacia Punto A desde B
        Girar(anguloBA);
        Mover(distanciaAB, false);
        break;

      case 6: 
      // Del angulo en direccion BA a el angulo en posicion inicial
      Mover(anguloA, false);
      Estado = 0; 
      Enable = false;
      break;

    }
  }else{
    // Verificar parámetros de entrada
    String Data = readDataBLE();
    if(Data!=""){
      //************************************
      // 1. Cargar parámetros de entrada
      //************************************
      if(parsearDatos(Data)){
        Enable = true;// Activa el robot
        apagarRojo();
        encenderVerde(); 

      }
    }
  }
}
float normalizarAngulo(float angulo) {
  angulo = fmod(angulo, 360.0f); // Asegura que esté entre 0–360
  if (angulo < 0) angulo += 360.0f;

  if (angulo > 180.0f) {
    angulo -= 360.0f; // Aquí derecha será negativa
  }

  return angulo;
}
/**/
/******************************************************************************/
//Ejemplo QRE1114
/******************************************************************************/
/**
void setup() {
  Serial.begin(115400);
  configurarSensores();
}

void loop() {
  bool objeto = detectarObjetoRedundante();
  //bool objeto = detectarObjetoPreciso(QRE_PIN);
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
char estado = 0;
void setup() {
  Serial.begin(115200);

  configurarMPU6500();
  ConfigMotor();
  ConfigEncoder(330, 6.2); // Encoder turncount , diameter
}

void loop() {
  
 
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');  // Lee hasta salto de línea
    if(data!=""){
      
      float number = data.toFloat();               // Convierte a float
      //Mover(number, false);
      Girar(number);
    }
  }
  
}
/**/

//*****************************************************************
// Ejemplo Covecion
//*****************************************************************
/**
void setup() {
  Serial.begin(9600);

  // Posición global del punto A
  GEX = 0.0; GEY = 0.0; GEA = 0.0;

  // Definición del sistema LOCAL respecto al global
  LEX = 0.0; LEY = 0.0; LEA = 0.0; // <-- Cámbialos según la orientación y posición del sistema local

  // Puntos B y C en coordenadas locales
  BX = 100.0; BY = 0.0;
  CX = 200.0; CY = 100.0;

  calcularTrayectoria();
  irA_C();
  volverA_A();  //ESTAS SON LAS FUNCIONES QUE SE DEBEN LLAMAR PARA CALCULAR Y VER LOS PUNTOS, para calcular los puntos y devolvere solo es necesario la primera y la ultima funcion
}

void loop() {}
/**/
