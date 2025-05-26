calData calib = { 0 };
AccelData accelData;
GyroData gyroData;

float offsetAcelX = 0.0;
float offsetAcelY = 0.0;
float offsetGyroZ = 0.0;

float velX = 0.0;
float velY = 0.0;
float posX = 0.0;
float posY = 0.0;
float angZ = 0.0;

bool giroActivo = false;
unsigned long tiempoQuietoAnterior = 0;
unsigned long tiempoAnterior = 0;
// Filtros
float bufferAcelX[N] = {0}, bufferAcelY[N] = {0};
int idxFiltro = 0;

// Estado de movimiento
bool movimientoX = false, movimientoY = false;
unsigned long tQuietoX = 0, tQuietoY = 0;

float acelX_hist[N] = {0};
float acelY_hist[N] = {0};
float gyroZ_hist[N] = {0};

int indx = 0;


//**********************************************
// Función de configuración del sensor MPU6500
// Inicializa el sensor, configura los rangos de aceleración y giroscopio,
// y realiza una calibración inicial para los ejes X, Y (acelerómetro) y Z (giroscopio).
//**********************************************
float stdAcelX = 0, stdAcelY = 0;
float stdVelX = 0, stdVelY = 0;



void configurarMPU6500() {
  Wire.begin();
  Wire.setClock(400000);

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error inicializando IMU: ");
    Serial.println(err);
    while (true);
  }

  IMU.setAccelRange(2);  // ±2g
  IMU.setGyroRange(500); // ±500 °/s

  // Calibración inicial de acelerómetro y giroscopio
  Serial.println("Calibrando acelerómetro y giróscopo...");
  delay(500);
  for (int i = 0; i < 100; i++) {
    IMU.update();
    IMU.getAccel(&accelData);
    IMU.getGyro(&gyroData);
    offsetAcelX += accelData.accelX;
    offsetAcelY += accelData.accelY;
    offsetGyroZ += gyroData.gyroZ;
    delay(10);
  }
  offsetAcelX /= 100.0;
  offsetAcelY /= 100.0;
  offsetGyroZ /= 100.0;

  tiempoAnterior = micros();
}

//**********************************************
// Función para actualizar valores integrados
// Debe llamarse periódicamente. Integra aceleración en X y Y para calcular velocidad
// y posición, además de integrar el giroscopio para estimar el ángulo Z.
//**********************************************
void actualizarValores() {
  unsigned long tiempoActual = micros();
  float deltaT = (tiempoActual - tiempoAnterior) / 1000000.0;
  tiempoAnterior = tiempoActual;

  IMU.update();
  IMU.getAccel(&accelData);
  IMU.getGyro(&gyroData);

  // --- Aceleración cruda ---
  float acelXraw = (accelData.accelX - offsetAcelX) * 981;
  float acelYraw = (accelData.accelY - offsetAcelY) * 981;

  // --- Filtro media móvil ---
  bufferAcelX[idxFiltro] = acelXraw;
  bufferAcelY[idxFiltro] = acelYraw;
  idxFiltro = (idxFiltro + 1) % N;

  float sumaX = 0, sumaY = 0;
  for (int i = 0; i < N; i++) {
    sumaX += bufferAcelX[i];
    sumaY += bufferAcelY[i];
  }

  float acelX = sumaX / N;
  float acelY = sumaY / N;

  // --- Movimiento en X ---
  if (abs(acelX) < UMBRAL_ACEL) {
    /*Serial.print("Menor al umbral/");
    Serial.print("Aceleracion X: "+String(acelX));
    Serial.println(" | Velocidad X: "+String(velX));*/
    if (!movimientoX) {
      if (millis() - tQuietoX > TIEMPO_QUIETO_MS) {
        velX = 0;
        acelX = 0;
      }
    } else {
      movimientoX = false;
      tQuietoX = millis();
    }
  } else {
    /*Serial.print("Mayor al umbral/");
    Serial.print("Aceleracion X: "+String(acelX));
    Serial.println(" | Velocidad X: "+String(velX));*/
    movimientoX = true;
    velX += acelX * deltaT;
    posX += velX * deltaT;
  }

  // --- Movimiento en Y ---
  if (abs(acelY) < UMBRAL_ACEL) {
    if (!movimientoY) {
      if (millis() - tQuietoY > TIEMPO_QUIETO_MS) {
        velY = 0;
        acelY = 0;
      }
    } else {
      movimientoY = false;
      tQuietoY = millis();
    }
  } else {
    movimientoY = true;
    velY += acelY * deltaT;
    posY += velY * deltaT;
  }

  // --- Giroscopio Z ---
  float gyroZ = gyroData.gyroZ - offsetGyroZ;
  if (abs(gyroZ) > 0.5) {
    angZ += gyroZ * deltaT;
    giroActivo = true;
    tiempoQuietoAnterior = millis();
  } else {
    if (giroActivo && (millis() - tiempoQuietoAnterior > 100)) {
      giroActivo = false;
    }
  }

  // Normalizar ángulo
  if (angZ > 180.0) angZ -= 360.0;
  if (angZ < -180.0) angZ += 360.0;
}


//**********************************************
// Función para obtener la posición X y Y del sensor
// Retorna un vector con las posiciones en X y Y (en centímetros),
// integradas a partir de la aceleración del sensor.
//**********************************************
std::vector<float> obtenerPosicionXY() {
  return { posX, posY };
}

//**********************************************
// Función para obtener la velocidad en el eje X
// Retorna el valor de la velocidad actual en el eje X (cm/s),
// calculada mediante la integración de la aceleración X.
//**********************************************
float obtenerVelocidadX() {
  return velX;
}

//**********************************************
// Función para obtener el ángulo Z
// Retorna el ángulo estimado (en grados) a partir del giroscopio,
// resultado de integrar la velocidad angular sobre el tiempo.
//**********************************************
float obtenerAnguloZ() {
  
  return angZ;
}

//***********************************************
// Filtro de promedio movil
//***********************************************
float filtroPromedio(float* buffer, float nuevaLectura) {
  buffer[indx] = nuevaLectura;
  float suma = 0;
  for (int i = 0; i < N; i++) suma += buffer[i];
  return suma / N;
}