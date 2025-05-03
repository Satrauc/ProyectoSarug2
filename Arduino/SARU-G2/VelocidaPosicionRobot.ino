#include <Wire.h>
#include <MPU9250_asukiaaa.h>

// Crear objeto del sensor
MPU9250_asukiaaa MPU6500;

float posX = 0, posY = 0;
float velX = 0, velY = 0;
float angleZ = 0;
unsigned long lastTimePos = 0;
bool isFirstPosCall = true;

std::vector<float> GetPositionAndAngle() {
  unsigned long currentTime = millis();

  if (isFirstPosCall) {
    lastTimePos = currentTime;
    isFirstPosCall = false;
    return {0.0, 0.0, 0.0};
  }

  float dt = (currentTime - lastTimePos) / 1000.0;

  angleZ += MPU6500.gyroZ() * dt; // en °. Para radianes: * DEG_TO_RAD
  velX += MPU6500.accelX() * dt * 9.81;
  velY += MPU6500.accelY() * dt * 9.81;
  posX += velX * dt;
  posY += velY * dt;

  lastTimePos = currentTime;

  return {posX, posY, angleZ};
}

std::vector<float> GetCurrentVelocity() {
  unsigned long currentTime = millis();

  if (isFirstVelCall) {
    lastTimeVel = currentTime;
    isFirstVelCall = false;
    return {0.0, 0.0}; // Inicialmente 0
  }

  float dt = (currentTime - lastTimeVel) / 1000.0; // tiempo en segundos

  // Integrar aceleración para obtener velocidad
  velX_ind += MPU6500.accelX() * dt * 9.81; // m/s² a m/s
  velY_ind += MPU6500.accelY() * dt * 9.81;

  lastTimeVel = currentTime;

  return {velX_ind, velY_ind};
}
void ConfigMpu6500(){
  Wire.begin(); // Para ESP32 puedes usar: Wire.begin(SDA_PIN, SCL_PIN);

  MPU6500.setWire(&Wire);
  MPU6500.beginAccel(); // Inicia acelerómetro
  MPU6500.beginGyro();  // Inicia giroscopio
}

void UpdateMPU6500(){
  MPU6500.accelUpdate();
  MPU6500.gyroUpdate();
}
