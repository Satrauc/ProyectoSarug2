/*=======================================
  Autores: Isabella Benavides Cifuentes
  ========================================*/
/*=======================================
                Librerias
  ========================================*/
#include <Simple_MPU6050.h> //https://github.com/ZHomeSlice/Simple_MPU6050
#include <Simple_Wire.h>    //https://github.com/ZHomeSlice/Simple_Wire
#include <Wire.h>

/*=======================================
           Variables Motor
  ========================================*/
const int Motor[] = {13, 12, 11, 10};
const int Pwm[] = {6, 5};
const int Stby = 8;
const int BaseSpeed = 150;

/*=======================================
          Variables Encoder
  ========================================*/
const int PinA = 3;
const int PinB = 2;
const int EncoderFullTurn = 5500; //Cambiar segun la ficha técnica
volatile int CountEncoderA = 0, CountEncoderB = 0;
volatile int DistanceEncoder = 0;
unsigned long FormerTime = 0;
float Rpm = 0.0;

/*=======================================
     Acelerometro y Giroscopio MPU6050
  ========================================*/
Simple_MPU6050 mpu;
float CurrentYaw = 0.0;
bool Turning = false;

/*=======================================
          Parametros necesarios
  ========================================*/
const int Perimeter = PI * 20.0; //El 20.0 hace referencia a el diametro de la rueda
float Distance = 0.0;

/*=======================================
          Modificar para Probar
  ========================================*/
float DistancePoint = 50; // Distancia a recorrer
float TargetAngle = 90;   // Angulo para girar

void setup() {
  //for (byte i = 0; i < 4; i++) pinMode(Motor[i], OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  mpu.begin();
  delay(1000);
  /*=======================================
             Parámetros de Motores
    ========================================*/
  for (int i = 0; i < 4; i++) {
    pinMode(Motor[i], OUTPUT);
  } for (int i = 0; i < 2; i++) {
    pinMode(Pwm[i], OUTPUT);
  }
  pinMode(Stby, OUTPUT);
  digitalWrite(Stby, HIGH); //Activa el driver
  /*=======================================
             Parámetros del Encoder
    ========================================*/
  pinMode(PinA, INPUT_PULLUP);
  pinMode(PinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinA), UpdateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(PinB), UpdateEncoderB, RISING);
  /*=======================================
             Parámetros del MPU6050
    ========================================*/


} void loop() {
  /*=======================================
           Calculo RMP & Distancia
    ========================================*/
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
  /*=======================================
           Movimiento Lineal Motor
    ========================================*/

  if (Distance <= DistancePoint && !Turning) {
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
  } else if (!Turning) {
    digitalWrite(Motor[0], LOW);
    digitalWrite(Motor[1], LOW);
    digitalWrite(Motor[2], LOW);
    digitalWrite(Motor[3], LOW);

    analogWrite(Pwm[0], 0);
    analogWrite(Pwm[1], 0);
    Turning = true;
    CurrentYaw = 0.0;
  }
    /*=======================================
           Movimiento Angular Motor
    ========================================*/
  if (Turning) {
    mpu.dmp_read_fifo();
    float gyroZ = mpu.gyro[2] / 131.0;
    CurrentYaw += gyroZ * 0.01;

    //Serial.print("Current Yaw: ");
    //Serial.println(CurrentYaw);

    if (abs(CurrentYaw) < TargetAngle) {
      digitalWrite(Motor[0], HIGH);
      digitalWrite(Motor[1], LOW);
      digitalWrite(Motor[2], LOW);
      digitalWrite(Motor[3], HIGH);

      analogWrite(Pwm[0], 150);
      analogWrite(Pwm[1], 150);
    } else {
      digitalWrite(Motor[0], LOW);
      digitalWrite(Motor[1], LOW);
      digitalWrite(Motor[2], LOW);
      digitalWrite(Motor[3], LOW);

      analogWrite(Pwm[0], 0);
      analogWrite(Pwm[1], 0);
      Turning = false;
      //Serial.println("¡Giro completado!");
    }
  }
}
void UpdateEncoderA() {
  CountEncoderA++;
  DistanceEncoder++;
}
void UpdateEncoderB() {
  CountEncoderB++;
  //DistanceEncoder++;
}
