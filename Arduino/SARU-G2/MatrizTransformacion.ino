#include <math.h>  
#include <Arduino.h> 

// Punto dado (en coordenadas homogéneas 3x1)
float pA[3] = {1.0, 2.0, 1.0}; 

// Matriz de transformación (3x3)
float T[3][3];

// Variables globales para almacenar datos necesarios para la inversa
float cosAlpha, sinAlpha;
float tx_global, ty_global;

// Función para inicializar la matriz de transformación T
void inicializarTransformacion(float alphaDeg, float tx, float ty) {
  float alphaRad = alphaDeg * (M_PI / 180.0);  // Conversión de grados a radianes

  // Calculamos coseno y seno del ángulo
  cosAlpha = cos(alphaRad);
  sinAlpha = sin(alphaRad);
  
  // Aqui se guardan los desplazamientos
  tx_global = tx;
  ty_global = ty;

  // Se define la matriz de transformacion
  T[0][0] = cosAlpha;
  T[0][1] = -sinAlpha;
  T[0][2] = tx;

  T[1][0] = sinAlpha;
  T[1][1] = cosAlpha;
  T[1][2] = ty;

  T[2][0] = 0.0;
  T[2][1] = 0.0;
  T[2][2] = 1.0;
}

// Función para transformar un punto usando T
void transformarPunto(float puntoIn[3], float puntoOut[3]) {
  for (int i = 0; i < 3; i++) {
    puntoOut[i] = 0.0;
    for (int j = 0; j < 3; j++) {
      puntoOut[i] += T[i][j] * puntoIn[j];
    }
  }
}

// Función para calcular la matriz inversa de T
void calcularInversa(float T_inv[3][3]) {
  T_inv[0][0] = cosAlpha;
  T_inv[0][1] = sinAlpha;
  T_inv[0][2] = -(tx_global * cosAlpha + ty_global * sinAlpha);

  T_inv[1][0] = -sinAlpha;
  T_inv[1][1] = cosAlpha;
  T_inv[1][2] = (tx_global * sinAlpha - ty_global * cosAlpha);

  T_inv[2][0] = 0.0;
  T_inv[2][1] = 0.0;
  T_inv[2][2] = 1.0;
}

// Función para transformar un punto usando la matriz inversa
void transformarPuntoInverso(float puntoIn[3], float puntoOut[3], float T_inv[3][3]) {
  for (int i = 0; i < 3; i++) {
    puntoOut[i] = 0.0;
    for (int j = 0; j < 3; j++) {
      puntoOut[i] += T_inv[i][j] * puntoIn[j];
    }
  }
}


/* PARA VISUALIZACION DE LAS TRANSFORMACIONES Y MATRICES

  // Para Inicializar transformación (ejemplo: 30°, tx=5, ty=3)
  inicializarTransformacion(30.0, 5.0, 3.0);

  // Para Imprimir matriz de transformación T
  Serial.println("Matriz de Transformación (T):");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Serial.print(T[i][j], 4); // 4 decimales
      Serial.print("\t");
    }
    Serial.println();
  }

  // Para Imprimir punto original
  Serial.print("\nPunto original: [");
  Serial.print(pA[0]); Serial.print(", ");
  Serial.print(pA[1]); Serial.print(", ");
  Serial.print(pA[2]); Serial.println("]");

  // Para Transformar punto e imprimir resultado
  float puntoTransformado[3];
  transformarPunto(pA, puntoTransformado);
  Serial.print("Punto transformado: [");
  Serial.print(puntoTransformado[0]); Serial.print(", ");
  Serial.print(puntoTransformado[1]); Serial.print(", ");
  Serial.print(puntoTransformado[2]); Serial.println("]");

  // Para Calcular e imprimir matriz inversa
  float T_inv[3][3];
  calcularInversa(T_inv);
  Serial.println("\nMatriz Inversa (T_inv):");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Serial.print(T_inv[i][j], 4);
      Serial.print("\t");
    }
    Serial.println();
  }

  // Para recuperar el punto original con inversa e imprimir
  float puntoRecuperado[3];
  transformarPuntoInverso(puntoTransformado, puntoRecuperado, T_inv);
  Serial.print("\nPunto recuperado: [");
  Serial.print(puntoRecuperado[0]); Serial.print(", ");
  Serial.print(puntoRecuperado[1]); Serial.print(", ");
  Serial.print(puntoRecuperado[2]); Serial.println("]");
}
*/
