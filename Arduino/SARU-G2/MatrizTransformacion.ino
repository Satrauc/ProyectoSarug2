// Resultados
float BxGlobal, ByGlobal;
float CxGlobal, CyGlobal;

float distAB, angAB;
float distBC, angBC;

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

// Paso 1 a 5: Preparar datos y calcular vectores/distancias/ángulos
void calcularTrayectoria(float BX, float BY, float LEA, float LEX, float LEY, float CX, float CY, float GEX, float GEY, float GEA) {
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
  /*Serial.println("Transformación completa:");
  Serial.print("Punto B Global: "); Serial.print(BxGlobal); Serial.print(", "); Serial.println(ByGlobal);
  Serial.print("Punto C Global: "); Serial.print(CxGlobal); Serial.print(", "); Serial.println(CyGlobal);
  Serial.print("Distancia AB: "); Serial.println(distAB);
  Serial.print("Ángulo AB: "); Serial.println(angAB);
  Serial.print("Distancia BC: "); Serial.println(distBC);
  Serial.print("Ángulo BC: "); Serial.println(angBC);
  Serial.print("Giro ang1 (de A a B): "); Serial.println(ang1);
  Serial.print("Giro ang2 (de B a C): "); Serial.println(ang2);*/
}

float normalizar(float ang) {
  while (ang < 0) ang += 360;
  while (ang >= 360) ang -= 360;
  return ang;
}

void volverA_A(float GEX, float GEY, float GEA) {
// Paso 1: De C a B
  float dxCB = BxGlobal - CxGlobal;
  float dyCB = ByGlobal - CyGlobal;
  float angCB = calcularAngulo(dxCB, dyCB);  // dirección de C → B
  ang3 = normalizar(angCB - angBC);    // rotación necesaria desde C hacia B

  // Paso 2: De B a A
  float dxBA = GEX - BxGlobal;
  float dyBA = GEY - ByGlobal;
  float angBA = calcularAngulo(dxBA, dyBA);  // dirección de B → A
  ang4 = normalizar(angBA - angCB);    // rotación desde B hacia A

  // Paso 3: Ajuste final en A para regresar a la orientación inicial
  ang5 = normalizar(GEA - angBA);      // rotación para igualar ángulo inicial en A

  /*
  Serial.println("RETORNO: De C a B...");
  Serial.print("Girar "); Serial.print(ang3); Serial.println(" grados");
  Serial.print("Avanzar "); Serial.print(distBC); Serial.println(" unidades");

  Serial.println("RETORNO: De B a A...");
  Serial.print("Girar "); Serial.print(ang4); Serial.println(" grados");
  Serial.print("Avanzar "); Serial.print(distAB); Serial.println(" unidades");*/
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
