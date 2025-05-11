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

unsigned long tiempoAnterior = 0;

//**********************************************
// Función de configuración del sensor MPU6500
// Inicializa el sensor, configura los rangos de aceleración y giroscopio,
// y realiza una calibración inicial para los ejes X, Y (acelerómetro) y Z (giroscopio).
//**********************************************
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
  for (int i = 0; i < 20; i++) {
    IMU.update();
    IMU.getAccel(&accelData);
    IMU.getGyro(&gyroData);
    offsetAcelX += accelData.accelX;
    offsetAcelY += accelData.accelY;
    offsetGyroZ += gyroData.gyroZ;
    delay(10);
  }
  offsetAcelX /= 20.0;
  offsetAcelY /= 20.0;
  offsetGyroZ /= 20.0;

  tiempoAnterior = micros();
}

//**********************************************
// Función para actualizar valores integrados
// Debe llamarse periódicamente. Integra aceleración en X y Y para calcular velocidad
// y posición, además de integrar el giroscopio para estimar el ángulo Z.
//**********************************************
void actualizarValores() {
  unsigned long tiempoActual = micros();
  float deltaT = (tiempoActual - tiempoAnterior) / 1000000.0; // en segundos
  tiempoAnterior = tiempoActual;

  IMU.update();
  IMU.getAccel(&accelData);
  IMU.getGyro(&gyroData);

  // Procesamiento aceleración en X
  float acelX = (accelData.accelX - offsetAcelX) * 977.86; // cm/s²
  if (abs(acelX) < 1.0) acelX = 0.0;

  velX += acelX * deltaT; // cm/s
  if (abs(velX) < 0.1) velX = 0.0;

  posX += velX * deltaT; // cm

  // Procesamiento aceleración en Y
  float acelY = (accelData.accelY - offsetAcelY) * 977.86; // cm/s²
  if (abs(acelY) < 1.0) acelY = 0.0;

  velY += acelY * deltaT; // cm/s
  if (abs(velY) < 0.1) velY = 0.0;

  posY += velY * deltaT; // cm

  // Procesamiento giroscopio Z
  float gyroZ = gyroData.gyroZ - offsetGyroZ; // °/s
  angZ += gyroZ * deltaT; // grados
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