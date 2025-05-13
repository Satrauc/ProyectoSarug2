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
float distanciaAB, anguloAB, angulo BA;
float distanciaBC, anguloBC, angulo CB, anguloA;
unsigned char Estado = 0;    // En Punto A = 0
                             // Hacia Punto B = 1
                             // Hacia Punto C = 2
                             // Espera remove planta = 3
                             // Hacia Punto B desde C = 4
                             // Hacia Punto A desde B = 5

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
    actualizarVariablesDesdeBLE();
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
      sendDataBLE("PX:" + String(PocisionActual[0], 3)+"|PY:" + String(PocisionActual[1], 3)+"|V:" + String(VelocidadActual, 3)+"|A:"+ String(VelocidadActual, 3)+"|R"+String(Rpms, 3));


      switch (Estado) {
        case 0:
          // En Punto A 

          
        //************************************
        // 3.1. Calculo de trayectorias
        //************************************
          calcularTrayectoria();
          distanciaAB = getDistancia_AB();
          anguloAB = getAngulo_AB();
          distanciaBC = getDistancia_BC();
          anguloBC = getAngulo_BC();
          Estado = 1; 

          break;

        case 1:
          // Hacia Punto B
          AngularMotor(anguloAB,255);
          LinealMotor(distanciaAB,115);
          delay(10000);
          Estado = 2; 
          break;

        case 2:
          // Hacia Punto C 
          AngularMotor(anguloBC,255);
          LinealMotor(distanciaBC,115);
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
          anguloBA = getAngulo_BA();
          anguloCB = getAngulo_CB();
          anguloA = getAngulo_A();

          Estado = 4; 
          break;
        
        case 4:
           // Hacia Punto B desde C
           AngularMotor(anguloCB,255);
           LinealMotor(distanciaBC,115);
           Estado = 5; 
          break;
        
        case 5:
          // Hacia Punto A desde B
          AngularMotor(anguloBA,255);
          LinealMotor(distanciaAB,115);
          break;
      }

        caso 6: 
        // Del angulo en direccion BA a el angulo en posicion inicial
        AnguloMotor(anguloA,255);
        Estado = 0; 
        Enable = false;
        break;

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

//*****************************************************************
// Ejemplo Motores
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
