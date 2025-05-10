#include <math.h>

// Posición del sistema LOCAL respecto al GLOBAL
float LEX, LEY, LEA;     // Traslación y ángulo del sistema LOCAL al GLOBAL

// Datos de entrada desde comunicación BLE
float GEX, GEY, GEA;     // Posición y orientación global del punto A (para cálculos de giros)
float BX, BY;            // Punto B en sistema LOCAL
float CX, CY;            // Punto C en sistema LOCAL

// Resultados
float BxGlobal, ByGlobal;
float CxGlobal, CyGlobal;

float distAB, angAB;
float distBC, angBC;

float ang1, ang2; // Ángulos relativos para girar

// Transforma punto local a global con la ecuación directa
void transformarPunto(float x, float y, float alphaDeg, float tx, float ty, float &xOut, float &yOut) {
  float alphaRad = alphaDeg * (PI / 180.0);
  xOut = x * cos(alphaRad) - y * sin(alphaRad) + tx;
  yOut = x * sin(alphaRad) + y * cos(alphaRad) + ty;
}

// Calcula ángulo entre dos puntos
float calcularAngulo(float dx, float dy) {
  float anguloRad = atan2(dy, dx);
  float anguloDeg = anguloRad * (180.0 / PI);
  if (anguloDeg < 0) anguloDeg += 360;
  return anguloDeg;
}

// Paso 1 a 5: Preparar datos y calcular vectores/distancias/ángulos
void calcularTrayectoria() {
  // 1. Transformar puntos locales B y C a coordenadas globales usando LEX, LEY, LEA
  transformarPunto(BX, BY, LEA, LEX, LEY, BxGlobal, ByGlobal);
  transformarPunto(CX, CY, LEA, LEX, LEY, CxGlobal, CyGlobal);

  // 2. Calcular vector A → B
  float dxAB = BxGlobal - GEX;
  float dyAB = ByGlobal - GEY;
  distAB = sqrt(dxAB * dxAB + dyAB * dyAB);
  angAB = calcularAngulo(dxAB, dyAB);

  // 3. Calcular vector B → C
  float dxBC = CxGlobal - BxGlobal;
  float dyBC = CyGlobal - ByGlobal;
  distBC = sqrt(dxBC * dxBC + dyBC * dyBC);
  angBC = calcularAngulo(dxBC, dyBC);

  // 4. Calcular giros (ángulos relativos)
  ang1 = angAB - GEA;
  if (ang1 < 0) ang1 += 360;

  ang2 = angBC - angAB;
  if (ang2 < 0) ang2 += 360;

  // Mostrar resultados
  Serial.println("Transformación completa:");
  Serial.print("Punto B Global: "); Serial.print(BxGlobal); Serial.print(", "); Serial.println(ByGlobal);
  Serial.print("Punto C Global: "); Serial.print(CxGlobal); Serial.print(", "); Serial.println(CyGlobal);
  Serial.print("Distancia AB: "); Serial.println(distAB);
  Serial.print("Ángulo AB: "); Serial.println(angAB);
  Serial.print("Distancia BC: "); Serial.println(distBC);
  Serial.print("Ángulo BC: "); Serial.println(angBC);
  Serial.print("Giro ang1 (de A a B): "); Serial.println(ang1);
  Serial.print("Giro ang2 (de B a C): "); Serial.println(ang2);
}

/* Paso 6: Mover robot A → B → C
void irA_C() {
  Serial.println("MOVIMIENTO: Ir de A a B...");
  Serial.print("Girar "); Serial.print(ang1); Serial.println(" grados");
  Serial.print("Avanzar "); Serial.print(distAB); Serial.println(" unidades");

  Serial.println("MOVIMIENTO: Ir de B a C...");
  Serial.print("Girar "); Serial.print(ang2); Serial.println(" grados");
  Serial.print("Avanzar "); Serial.print(distBC); Serial.println(" unidades");
} */

// Paso 7: Mover robot de regreso C → B → A
void volverA_A() {
  float ang3 = angAB - angBC;
  if (ang3 < 0) ang3 += 360;
  float ang4 = GEA - angAB;
  if (ang4 < 0) ang4 += 360;

/*
  Serial.println("RETORNO: De C a B...");
  Serial.print("Girar "); Serial.print(ang3); Serial.println(" grados");
  Serial.print("Avanzar "); Serial.print(distBC); Serial.println(" unidades");

  Serial.println("RETORNO: De B a A...");
  Serial.print("Girar "); Serial.print(ang4); Serial.println(" grados");
  Serial.print("Avanzar "); Serial.print(distAB); Serial.println(" unidades");
} */

/* PARA HACER PRUEBAS Y SIMULAR
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

void loop() {
  
}*/

