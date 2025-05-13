#include <math.h>

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
  // 1. Transformar puntos locales B y C a coordenadas globales
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

  // 4. Calcular giros relativos
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

// Definición de epsilon para comparaciones de ángulos
const float EPSILON = 1e-2;

// Función para normalizar ángulos entre 0 y 360
float normalizar(float ang) {
  while (ang < 0) ang += 360;
  while (ang >= 360) ang -= 360;
  return ang;
}

// Función para imprimir giro con dirección mínima e indicando izquierda o derecha
void imprimirGiro(float ang) {
  ang = normalizar(ang);
  
  // Aplicar epsilon: si el ángulo es muy pequeño, no girar
  if (abs(ang) < EPSILON || abs(ang - 360) < EPSILON) {
    Serial.println("No es necesario girar.");
  }
  else if (ang <= 180) {
    Serial.print("Girar a la IZQUIERDA: ");
    Serial.print(ang);
    Serial.println(" grados");
  } else {
    Serial.print("Girar a la DERECHA: ");
    Serial.print(360 - ang);
    Serial.println(" grados");
  }
}


// Función para ir desde A hasta C (ida)
void irA_C() {
  Serial.println("MOVIMIENTO: Ir de A a B...");
  imprimirGiro(ang1);
  Serial.print("Avanzar "); Serial.print(distAB); Serial.println(" centimetros");

  Serial.println("MOVIMIENTO: Ir de B a C...");
  imprimirGiro(ang2);
  Serial.print("Avanzar "); Serial.print(distBC); Serial.println(" centimetros");
} //ESTA FUNCION ES SOLO PARA IMPRIMIR LAS DISTANCIAS Y GIROS QUE DEBERIA AVANZAR

// Función principal para volver al punto A
void volverA_A() {
  // Paso 1: De C a B
  float dxCB = BxGlobal - CxGlobal;
  float dyCB = ByGlobal - CyGlobal;
  float angCB = calcularAngulo(dxCB, dyCB);  // dirección de C → B
  float ang3 = normalizar(angCB - angBC);    // rotación necesaria desde C hacia B

  // Paso 2: De B a A
  float dxBA = GEX - BxGlobal;
  float dyBA = GEY - ByGlobal;
  float angBA = calcularAngulo(dxBA, dyBA);  // dirección de B → A
  float ang4 = normalizar(angBA - angCB);    // rotación desde B hacia A

  // Paso 3: Ajuste final en A para regresar a la orientación inicial
  float ang5 = normalizar(GEA - angBA);      // rotación para igualar ángulo inicial en A

  // OUTPUT
  Serial.println("RETORNO: De C a B...");
  imprimirGiro(ang3);
  Serial.print("Avanzar "); Serial.print(distBC); Serial.println(" unidades");

  Serial.println("RETORNO: De B a A...");
  imprimirGiro(ang4);
  Serial.print("Avanzar "); Serial.print(distAB); Serial.println(" unidades");

  Serial.println("RETORNO: En A...");
  imprimirGiro(ang5);
}

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
}
*/ /// IMPORTANTE:
// La función actualizarVariablesDesdeBLE() NO debe estar aquí.
// Debe estar en el módulo de comunicaciones, donde se reciben los datos BLE.
// Solo se llama desde allá después de parsear los datos en el main.

// PRUEBA 
/*void setup() {
  Serial.begin(9600);
setupBLE();  // Inicia el BLE

  // Espera hasta recibir datos por primera vez
  while (!deviceConnected || rxValue.length() == 0) {
    delay(100);
  }

  // Procesar solo una vez
  parsearDatos(rxValue);
  actualizarVariablesDesdeBLE();  // <--- Esta función copia los datos a LEX, LEY, etc.
 
  rxValue = "";
 
  calcularTrayectoria();  // A → B → C
  irA_C();                // Movimiento hacia adelante
  volverA_A();            // Regreso al punto inicial

  
}  // ESTO ES UN EJEMPLO DE LO QUE DE BERIA IR EN EL SETUP PARA INICIALIZAR LAS VARIABLES POR MEDIO DE BLE Y QEU FUNCIONEN LAS FUNCIONES DE LAS MATRICES 

void loop() {
  // Nada
}*/


