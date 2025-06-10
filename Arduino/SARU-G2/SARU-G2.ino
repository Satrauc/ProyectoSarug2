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

// MOVIMIENTO DE MOTORES
  #define VELOCIDAD_ANGULAR 80
  #define VELOCIDAD_LINEAL 80
  #define ERROR_ANGULAR  1.0

/***************************************************************/
// Variables Main SARU
/***************************************************************/

// Arreglos para almacenar valores
float Global[3];  // GEX, GEY, GEA
float Local[3];   // LEX, LEY, LEA
float PuntoB[2];  // BX, BY
float PuntoC[2];  // CX, CY

/**/
bool Enable = false;        // Verifica el estado del robot Saru-G2
std::vector<float> PocisionActual = {0.0, 0.0};
int detectada = 0;
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
void Actualizar_EnvioDatos(){
  actualizarValores();    // MPU6500
  Rpms = RmpDistance();   // Encoder
  PocisionActual =  obtenerPosicionXY();// MPU6500
  VelocidadActual = obtenerVelocidadX();// MPU6500
  AnguloActual = obtenerAnguloZ();      // MPU6500
  

  //************************************
  // 3.2. Envio de datos a matlab
  //************************************
  String mensaje = "PX:" + String(PocisionActual[0], 3)+"|PY:" + String(PocisionActual[1], 3)+"|V:" + String(VelocidadActual, 3)+"|A:"+ String(AnguloActual, 3)+"|R:"+String(Rpms, 3);
  
  sendDataBLE(mensaje);// Envio de datos del robot 
}
/**/
void setup() {
    Serial.begin(115200);

    // Inicialización de hardware y módulos
    setupBLE();
    configurarSensores();
    configuracionLeds();
    ConfigMotor();
    ConfigEncoder(350, 6.0);
    configurarMPU6500();
    encenderRojo(); 
    
    // Proceso leds
    // 1- Inicia con led rojo y verde apagado hasta conexion
    // 2- Al conectar, Led rojo y verde activos esperando ingreso de datos
    // 3- Con datos apaga el rojo y parpadea en verde esperando planta
    // 3- con planta el verde activo en proceso de A a B y a C
    // 4- Parpadea rojo y apaga verde hasta retirar la planta 
    // 5- led rojo apagado y verde activo en proceso de C a B y a A
    // 6- Vuelve a led rojo y verde en espera de datos de matlab
    
}
void loop() {
    /**/
    if(Enable){// Si el robot esta conextado y activo 
      // Ya hay datos en los arreglos      

      //************************************
      // 2. ¿Planta ubicada correctamente?
      //************************************
      if(detectada == 0){
        while (!detectarObjetoRedundante()) {
          int espera = 1000;
          encenderVerde(); //Parpadeo de led verde en espera de planta
          delay(espera * 0.5);
          apagarVerde();
          delay(espera * 0.5);
        }
        detectada = 1;
      }
      encenderVerde();

      Actualizar_EnvioDatos();

      switch (Estado) {
        case 0:
          // En Punto A  
        //************************************
        // 3.1. Calculo de trayectorias
        //************************************
          //sendDataBLE("---Caso 0---");
          //delay(1000);
          calcularTrayectoria();
          distanciaAB = getDistancia_AB();
          anguloAB = normalizarAngulo(getAngulo_AB());
          distanciaBC = getDistancia_BC();
          anguloBC = normalizarAngulo(getAngulo_BC());
          /*sendDataBLE("Angulo 1 AB: "+String(anguloAB));
          delay(1000);
          sendDataBLE("Distancia 1 AB: "+String(distanciaAB));
          delay(1000);
          sendDataBLE("Angulo 2 BC: "+String(anguloBC));
          delay(1000);
          sendDataBLE("Distancia 2 BC: "+String(distanciaBC));
          delay(5000);*/
          Estado = 1; 
          break;

        case 1:
          // Hacia Punto B
          //sendDataBLE("---Caso 1---");
          //delay(1000);
          //sendDataBLE("Giro");
          //delay(1000);
          Girar(anguloAB);
          ResetEncoder();
          //delay(5000);
          //sendDataBLE("Avanzar");
          //delay(1000);
          Mover(distanciaAB,false);
          ResetEncoder();
          delay(10000);//10000
          Estado = 2; 
          break;

        case 2:
          // Hacia Punto C 
          /*sendDataBLE("---Caso 2---");
          delay(1000);
          sendDataBLE("Giro");
          delay(1000);*/
          Girar(anguloBC);
          ResetEncoder();
          //delay(5000);
          //sendDataBLE("Avanzar");
          //delay(1000);
          Mover(distanciaBC,false);
          ResetEncoder();
          //delay(5000);
          Estado = 3; 
          break;

        case 3:
          //************************************
          // 4. Espera remove planta
          //************************************
          //sendDataBLE("---Caso 3---");
          //delay(1000);
          while (detectarObjetoRedundante()) {
            int espera = 500;
            encenderRojo();//Parpadeo de led rojo  en espera de retiro de planta
            delay(espera/2);
            apagarRojo();
            delay(espera/2);
          }
          delay(1000);
          //************************************
          // 4. Calculo de trayectoria de regreso
          //************************************
          volverA_A();
          anguloBA = normalizarAngulo(getAngulo_BA());
          anguloCB = normalizarAngulo(getAngulo_CB());
          anguloA = normalizarAngulo(getAngulo_A());
          /*delay(1000);
          sendDataBLE("Angulo 3 BA: "+String(anguloBA));
          delay(1000);
          sendDataBLE("Distancia 3 BC: "+String(distanciaBC));
          delay(1000);
          sendDataBLE("Angulo 4 CB: "+String(anguloCB));
          delay(1000);
          sendDataBLE("Distancia 4 AB: "+String(distanciaAB));
          delay(1000);
          sendDataBLE("Angulo 5 A: "+String(anguloA));
          delay(1000);*/

          Estado = 4; 
          break;
        
        case 4:
           // Hacia Punto B desde C
          //sendDataBLE("---Caso 4---");
          //delay(1000);
          //sendDataBLE("Giro");
          //delay(1000);
          Girar(anguloCB);
          ResetEncoder();
          //delay(1000);
          //sendDataBLE("Avanzar");
          //delay(1000);
          Mover(distanciaBC,false);
          ResetEncoder();
          //delay(5000);
          Estado = 5; 
          break;
        
        case 5:
          // Hacia Punto A desde B
          //sendDataBLE("---Caso 5---");
          //delay(1000);
          //sendDataBLE("Giro");
          //delay(1000);
          Girar(anguloBA);
          ResetEncoder();
          //delay(1000);
          //sendDataBLE("Avanzar");
          //delay(1000);
          Mover(distanciaAB,false);
          ResetEncoder();
          //delay(5000);
          Estado = 6; 
          break;
        case 6: 
          // Del angulo en direccion BA a el angulo en posicion inicial
          /*sendDataBLE("---Caso 6---");
          delay(1000);
          sendDataBLE("Giro");
          delay(1000);*/
          Girar(anguloA);
          ResetEncoder();
          encenderRojo();
          apagarVerde();
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
        }else{
          encenderVerde();
          delay(1000);
        }
      }
    }
    /**/
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
  /*String Data = readDataBLE();
  if(Data!=""){
    bool hola = parsearDatos(Data);
    Serial.println(Data);
    imprimirDatos() ;
  }
  else {
    
  }
  //sendDataBLE("PX:12.345|PY:67.890|V:3.457|A:90.000|R:1500.123");
  //sendDataBLE("Hola desde ESP32");
  delay(1000);
}
/**/
/******************************************************************************/
//Ejemplo MPU6500
/******************************************************************************/
/**/
/**/
//*****************************************************************
// Ejemplo Motores
//*****************************************************************
/**
void setup() {
  Serial.begin(115200);

  configurarMPU6500();
  ConfigMotor();
  ConfigEncoder(350, 6.0); // Encoder turncount , diameter
}
void loop() {

  //MoverMotores(-100, -100);
 /**
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');  // Lee hasta salto de línea
    if(data!=""){
      
      float number = data.toFloat();               // Convierte a float
      //Mover(number, false);
      //ResetEncoder();
      Girar(number);
    }
  }
  /**
}
/**/
//*****************************************************************
// Ejemplo Coversion
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
