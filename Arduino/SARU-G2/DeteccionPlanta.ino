const int sensores[] = {0, 1, 2};  // Usa de 1 a 4 sensores
const int cantidadSensores = 3;
const int umbral = 500;

// Configuración del pin
void ConfigQRE1114(int QRE_PIN){
  pinMode(QRE_PIN, INPUT);

}
// Configuración de múltiples sensores QRE1113/QRE1114
void configurarSensores(const int sensores[], int cantidad) {
  for (int i = 0; i < cantidad; i++) {
    pinMode(sensores[i], INPUT);
  }
}

// Función para leer múltiples veces un solo sensor y calcular una media más precisa
int lecturaPrecisa(int QRE_PIN, int repeticiones = 10) {
  long suma = 0;
  for (int i = 0; i < repeticiones; i++) {
    suma += analogRead(QRE_PIN);
    delay(2);  // Pequeño retardo para estabilidad
  }
  return suma / repeticiones;
}

// Función para detectar objeto con un solo sensor usando múltiples lecturas
bool detectarObjetoPreciso(int UMBRAL, int QRE_PIN) {
  int promedio = lecturaPrecisa(QRE_PIN);
  return promedio < UMBRAL;
}

// Función principal para detección con 1 hasta 4 sensores
bool detectarObjetoRedundante(int UMBRAL, const int sensores[], int cantidad) {
  int detecciones = 0;

  for (int i = 0; i < cantidad; i++) {
    if (cantidad == 1) {
      if (detectarObjetoPreciso(UMBRAL, sensores[i])) {
        detecciones++;
      }
    } else {
      if (analogRead(sensores[i]) < UMBRAL) {
        detecciones++;
      }
    }
  }

  // Si más del 50% de sensores detectan, consideramos que hay un objeto
  return (detecciones >= (cantidad + 1) / 2);
}
