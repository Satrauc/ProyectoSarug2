// Resultados
float BxGlobal, ByGlobal;
float CxGlobal, CyGlobal;

float distAB, angAB;
float distBC, angBC, angCB;

float ang1, ang2, ang3, ang4, ang5; // Ángulos relativos para girar

// Transforma punto local a global con la ecuación directa
void transformarPunto(float x, float y, float alphaDeg, float tx, float ty, float &xOut, float &yOut) {
  float alphaRad = alphaDeg * (PI / 180.0);

  // Matriz de transformación homogénea 3x3
  float T[3][3] = {
    {cos(alphaRad), -sin(alphaRad), tx},
    {sin(alphaRad),  cos(alphaRad), ty},
    {0,              0,             1}
  };

  // Vector columna del punto local en coordenadas homogéneas
  float puntoLocal[3] = {x, y, 1};

  // Resultado de la multiplicación T * puntoLocal
  float resultado[3] = {0, 0, 0};
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      resultado[i] += T[i][k] * puntoLocal[k];
    }
  }

  // Asignar las coordenadas transformadas
  xOut = resultado[0];
  yOut = resultado[1];
}
// Calcula ángulo entre dos puntos
float calcularAngulo(float dx, float dy) {
  float anguloRad = atan2(dy, dx);
  float anguloDeg = anguloRad * (180.0 / PI);
  if (anguloDeg < 0) anguloDeg += 360;
  return anguloDeg;
}
void calcularTrayectoria() {
  /**
  Equivalencias del arreglo de datos
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
  /**/
  transformarPunto(PuntoB[0], PuntoB[1], Local[2], Local[0], Local[1], BxGlobal, ByGlobal);
  transformarPunto(PuntoC[0], PuntoC[1], Local[2], Local[0], Local[1], CxGlobal, CyGlobal);

  // 2. Calcular vector A → B
  float dxAB = BxGlobal - Global[0];
  float dyAB = ByGlobal - Global[1];
  distAB = sqrt(dxAB * dxAB + dyAB * dyAB);
  angAB = calcularAngulo(dxAB, dyAB);

  // 3. Calcular vector B → C
  float dxBC = CxGlobal - BxGlobal;
  float dyBC = CyGlobal - ByGlobal;
  distBC = sqrt(dxBC * dxBC + dyBC * dyBC);
  angBC = calcularAngulo(dxBC, dyBC);

  // 4. Calcular giros (ángulos relativos)
  ang1 = angAB - Global[2];
  if (ang1 < 0) ang1 += 360;

  ang2 = angBC - angAB;
  if (ang2 < 0) ang2 += 360;
}
float normalizar(float ang) {
  if (ang < 0) ang += 360;
  //if (ang >= 360) ang -= 360;
  return ang;
}
void volverA_A() {
  /*
  Equivalencias del arreglo de datos
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
  */
  // Paso 1: De C a B
  float dxCB = BxGlobal - CxGlobal;
  float dyCB = ByGlobal - CyGlobal;
  float angCB = calcularAngulo(dxCB, dyCB);  // dirección de C → B
  ang3 = normalizar(angCB - angBC);    // rotación necesaria desde C hacia B

  // Paso 2: De B a A
  float dxBA = Global[0] - BxGlobal;
  float dyBA = Global[1] - ByGlobal;
  float angBA = calcularAngulo(dxBA, dyBA);  // dirección de B → A 
  ang4 = normalizar(angBA - angCB);    // rotación desde B hacia A

  // Paso 3: Ajuste final en A para regresar a la orientación inicial
  ang5 = normalizar(Global[2] - angBA);      // rotación para igualar ángulo inicial en A

} 

float getDistancia_AB(){
  return distAB;
}
float getAngulo_AB(){
  return ang1;
}
float getDistancia_BC(){
  return distBC;
}
float getAngulo_BC(){
  return ang2;
}

float getAngulo_CB(){
  return ang3;
}

float getAngulo_BA(){
  return ang4;
}

float getAngulo_A(){
  return ang5;
}
/**/