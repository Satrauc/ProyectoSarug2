#include <Wire.h>
#include <Simple_Wire.h>
#include <Simple_MPU6050.h>
// ==== Inicializaciones ====
void iniciarMotores() {
  Wire.begin();
  for (int i = 0; i < 4; i++) pinMode(Motor[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(Pwm[i], OUTPUT);
  pinMode(Stby, OUTPUT);
  digitalWrite(Stby, HIGH); // Activa el driver
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

// ==== Encoder ====
void UpdateEncoderA() {
  CountEncoderA++;
  DistanceEncoder++;
}

void UpdateEncoderB() {
  CountEncoderB++;
}

// ==== Lógica principal ====
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

// ==== Movimiento Motor ====
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
