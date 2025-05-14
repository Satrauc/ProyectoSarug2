const int sensores[] = {36, 39, 32, 33};  // Usa de 1 a 4 sensores
const int cantidadSensores = 4;
const int umbral = 500;


// Configuración de múltiples sensores QRE1113/QRE1114
void configurarSensores() {
  for (int i = 0; i < cantidadSensores; i++) {
    pinMode(sensores[i], INPUT);
  }
}

// Función para leer múltiples veces un solo sensor y calcular una media más precisa
int lecturaPrecisa(int Sensor, int repeticiones = 10) {
  long suma = 0;
  for (int i = 0; i < repeticiones; i++) {
    suma += analogRead(Sensor);
    delay(2);  // Pequeño retardo para estabilidad
  }
  return suma / repeticiones;
}

// Función para detectar objeto con un solo sensor usando múltiples lecturas
bool detectarObjetoPreciso(int Sensor = QRE_PIN) {
  int promedio = lecturaPrecisa(Sensor);
  return promedio < umbral;
}

// Función principal para detección con 1 hasta 4 sensores
bool detectarObjetoRedundante() {
  int detecciones = 0;

  for (int i = 0; i < cantidadSensores; i++) {
    if (cantidadSensores == 1) {
      if (detectarObjetoPreciso( sensores[i])) {
        detecciones++;
      }
    } else {
      if (analogRead(sensores[i]) < umbral) {
        detecciones++;
      }
    }
  }

  // Si más del 50% de sensores detectan, consideramos que hay un objeto
  return (detecciones >= (cantidadSensores + 1) / 2);
}
