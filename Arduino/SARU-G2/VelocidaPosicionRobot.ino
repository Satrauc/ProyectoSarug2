#include <Wire.h>
#include <MPU9250_asukiaaa.h>

// Crear objeto del sensor
MPU9250_asukiaaa MPU6500;

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