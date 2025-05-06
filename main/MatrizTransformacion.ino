#include <math.h>

float T[3][3];
float puntoOut[3];
float MatrizInit[3]; //Tx, Ty, Alpha
float CosAlpha, SinAlpha, Tx, Ty;
float puntoTransformado[3];

void inicializarTransformacion(float alphaDeg, float traslacionX, float traslacionY) {
  float alphaRad = alphaDeg * (M_PI / 180.0);  // Grados a radianes

  CosAlpha = cos(alphaRad);
  SinAlpha = sin(alphaRad);
  Tx = traslacionX;
  Ty = traslacionY;

  T[0][0] = CosAlpha;
  T[0][1] = -SinAlpha;
  T[0][2] = Tx;

  T[1][0] = SinAlpha;
  T[1][1] = CosAlpha;
  T[1][2] = Ty;

  T[2][0] = 0.0;
  T[2][1] = 0.0;
  T[2][2] = 1.0;
}
// Función para transformar un punto
void transformarPunto(float puntoIn[3]) {
  for (int i = 0; i < 3; i++) {
    puntoOut[i] = 0.0;
    Serial.println("Punto Transformado");
    for (int j = 0; j < 3; j++) {
      puntoOut[i] += T[i][j] * puntoIn[j];
    }
  }
  //return puntoOut[3];
}
void PrintMatriz(float MatrizInit[]) {
  Serial.begin(9600);  

  transformarPunto(MatrizInit);  

  Serial.println("Punto Transformado:");
  for (int i = 0; i < 3; i++) {
    Serial.println(puntoOut[i]);  
  }
}


