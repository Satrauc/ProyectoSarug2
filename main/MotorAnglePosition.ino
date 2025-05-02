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
int Motor[4];
//int Pwm[2];
int BaseSpeed = 40;

/*=======================================
          Variables Encoder
  ========================================*/
int EncoderFullTurn; //Cambiar segun la ficha técnica
volatile int CountEncoderA = 0, CountEncoderB = 0;
volatile int DistanceEncoder = 0;
unsigned long FormerTime = 0;
float Rpm = 0.0;

/*=======================================
     Acelerometro y Giroscopio MPU6050
  ========================================*/
Simple_MPU6050 mpu;
float CurrentYaw = 0.0;

/*=======================================
          Parametros necesarios
  ========================================*/
bool Turning = false;
float Distance = 0.0;
int Perimeter;
/*=======================================
          Modificar para Probar
  ========================================*/
//float DistancePoint = 50; // Distancia a recorrer
//float TargetAngle = 90;   // Angulo para girar

/*=======================================
       Comfigutacion de los Motores
  ========================================*/
void ConfigMotor(int Motor[], int Pwm[], int Stby) {
  for (byte i = 0; i < 4; i++) pinMode(Motor[i], OUTPUT);
  for (byte i = 0; i < 2; i++) pinMode(Pwm[i], OUTPUT);
  pinMode(Stby, OUTPUT);
  digitalWrite(Stby, HIGH);

}
/*=======================================
       Comfigutacion de los Encoder's
  ========================================*/
void Encoder(int PinAB[], int EncoderFullTurn, int Diameter) {
  const int Perimeter = PI * Diameter; //El 20.0 hace referencia a el diametro de la rueda
  for (byte i = 0; i < 2; i++) pinMode(PinAB[i], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinAB[0]), UpdateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(PinAB[1]), UpdateEncoderB, RISING);

}
/*=======================================
         Calculo RMP & Distancia
  ========================================*/
void RmpDistance() {
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
/*=======================================
       Movimiento Lineal del Motor
  ========================================*/
void LinealMotor(int DistancePoint, int BaseSpeed) {
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
}
/*=======================================
     Movimiento Angular Motor
  ========================================*/
void AngularMotor(float TargetAngle, int BaseSpeed) {
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

      analogWrite(Pwm[0], BaseSpeed);
      analogWrite(Pwm[1], BaseSpeed);
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
/*=======================================
              Conteo Encoder
  ========================================*/
void UpdateEncoderA() {
  CountEncoderA++;
  DistanceEncoder++;
}
void UpdateEncoderB() {
  CountEncoderB++;
  //DistanceEncoder++;
}
