#include "robot.h"

// Variables Motor
const int Motor[] = {13, 12, 11, 10};
const int Pwm[] = {6, 5};
const int Stby = 8;
const int BaseSpeed = 150;

// Variables Encoder
const int PinA = 3;
const int PinB = 2;
const int EncoderFullTurn = 5500;
volatile int CountEncoderA = 0, CountEncoderB = 0;
volatile int DistanceEncoder = 0;
unsigned long FormerTime = 0;
float Rpm = 0.0;

// Acelerómetro y Giroscopio
Simple_MPU6050 mpu;
float CurrentYaw = 0.0;
bool Turning = false;

// Parámetros
const int Perimeter = PI * 20.0;
float Distance = 0.0;

// Modificar para Probar
float DistancePoint = 50;
float TargetAngle = 90;

// Funciones
void iniciarMotores() {
  Wire.begin();
  for (int i = 0; i < 4; i++) pinMode(Motor[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(Pwm[i], OUTPUT);
  pinMode(Stby, OUTPUT);
  digitalWrite(Stby, HIGH);
}

void iniciarEncoder() {
  pinMode(PinA, INPUT_PULLUP);
  pinMode(PinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinA), UpdateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(PinB), UpdateEncoderB, RISING);
}

void iniciarMPU() {
  mpu.begin();
  delay(1000);
}

void calcularRPMyDistancia() {
  unsigned long CurrentTime = millis();
  if (CurrentTime - FormerTime >= 1000) {
    noInterrupts();
    int PulseRead = CountEncoderA;
    int PulseDistance = DistanceEncoder;
    CountEncoderA = 0;
    interrupts();
    Rpm = (PulseRead / (float)EncoderFullTurn) * 60.0;
    Distance = (DistanceEncoder / (float)EncoderFullTurn) * Perimeter;
    Serial.println("RPM: " + String(Rpm));
    Serial.println("Distancia: " + String(Distance) + " Cm");
    FormerTime = CurrentTime;
  }
}

void moverse() {
  if (Distance <= DistancePoint && !Turning) {
    avanzar();
  } else if (!Turning) {
    detenerse();
    comenzarGiro();
  }

  if (Turning) {
    girar();
  }
}

void UpdateEncoderA() {
  CountEncoderA++;
  DistanceEncoder++;
}

void UpdateEncoderB() {
  CountEncoderB++;
}

// Internas
void avanzar() {
  digitalWrite(Motor[0], HIGH);
  digitalWrite(Motor[1], LOW);
  digitalWrite(Motor[2], HIGH);
  digitalWrite(Motor[3], LOW);

  if (CountEncoderA > CountEncoderB + 5) {
    analogWrite(Pwm[0], BaseSpeed - 10);
    analogWrite(Pwm[1], BaseSpeed + 10);
  } else if (CountEncoderB > CountEncoderA + 5) {
    analogWrite(Pwm[0], BaseSpeed + 10);
    analogWrite(Pwm[1], BaseSpeed - 10);
  } else {
    analogWrite(Pwm[0], BaseSpeed);
    analogWrite(Pwm[1], BaseSpeed);
  }
}

void detenerse() {
  digitalWrite(Motor[0], LOW);
  digitalWrite(Motor[1], LOW);
  digitalWrite(Motor[2], LOW);
  digitalWrite(Motor[3], LOW);
  analogWrite(Pwm[0], 0);
  analogWrite(Pwm[1], 0);
}

void comenzarGiro() {
  Turning = true;
  CurrentYaw = 0.0;
}

void girar() {
  mpu.dmp_read_fifo();
  float gyroZ = mpu.gyro[2] / 131.0;
  CurrentYaw += gyroZ * 0.01;

  if (abs(CurrentYaw) < TargetAngle) {
    digitalWrite(Motor[0], HIGH);
    digitalWrite(Motor[1], LOW);
    digitalWrite(Motor[2], LOW);
    digitalWrite(Motor[3], HIGH);
    analogWrite(Pwm[0], 150);
    analogWrite(Pwm[1], 150);
  } else {
    detenerse();
    Turning = false;
  }
}
