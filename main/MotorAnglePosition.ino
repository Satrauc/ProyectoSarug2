/*=======================================
  Autor: Isabella Benavides Cifuentes
  ========================================*/

#include <Simple_MPU6050.h> // https://github.com/ZHomeSlice/Simple_MPU6050
#include <Simple_Wire.h>    
#include <Wire.h>

//int Motor[4];
//int Pwm[2]; 

/*=======================================
           Pines del Motor TB6612
  ========================================*/
const int pwmChannelA = 0;
const int pwmChannelB = 1;
int BaseSpeed = 100;

/*=======================================
          Variables Encoder
  ========================================*/
int EncoderFullTurn;
volatile int CountEncoderA = 0, CountEncoderB = 0;
volatile int DistanceEncoder = 0;
unsigned long FormerTime = 0;
float Rpm = 0.0;

/*=======================================
     Acelerómetro y Giroscopio MPU6050
  ========================================*/
Simple_MPU6050 mpu;
float CurrentYaw = 0.0;

/*=======================================
          Parámetros necesarios
  ========================================*/
bool Turning = false;
float Distance = 0.0;
int Perimeter;

/*=======================================
       Configuración de los Motores
  ========================================*/
void ConfigMotor(int Motor[], int Pwm[], int Stby) {
  for (byte i = 0; i < 4; i++) pinMode(Motor[i], OUTPUT);
  pinMode(Stby, OUTPUT);
  digitalWrite(Stby, HIGH);
  ledcSetup(pwmChannelA, 1000, 8);
  ledcAttachPin(Pwm[0], pwmChannelA);
  ledcSetup(pwmChannelB, 1000, 8);
  ledcAttachPin(Pwm[1], pwmChannelB);
}

/*=======================================
       Configuración de los Encoders
  ========================================*/
void Encoder(int PinAB[], int encoderTurnCount, float Diameter) {
  EncoderFullTurn = encoderTurnCount;
  Perimeter = PI * Diameter;
  for (byte i = 0; i < 2; i++) pinMode(PinAB[i], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinAB[0]), UpdateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(PinAB[1]), UpdateEncoderB, RISING);
}

void ConfigMpu(){
  
}

/*=======================================
         Cálculo RPM & Distancia
  ========================================*/
void RmpDistance() {
  unsigned long CurrentTime = millis();
  if (CurrentTime - FormerTime >= 100) {
    noInterrupts();
    int PulseRead = CountEncoderA;
    int PulseDistance = DistanceEncoder;
    CountEncoderA = 0;
    interrupts();
    Rpm = (PulseRead / (float)EncoderFullTurn) * 60.0;
    Distance = (PulseDistance / (float)EncoderFullTurn) * Perimeter;
    Serial.println("RPM: " + String(Rpm));
    Serial.println("Distancia: " + String(Distance) + " Cm");
    FormerTime = CurrentTime;
  }
}

/*=======================================
       Movimiento Lineal del Motor
  ========================================*/
void LinealMotor(float DistancePoint, int BaseSpeed) {
  if (Distance <= DistancePoint && !Turning) {
    digitalWrite(Motor[0], HIGH);
    digitalWrite(Motor[1], LOW);
    digitalWrite(Motor[2], HIGH);
    digitalWrite(Motor[3], LOW);

    if (CountEncoderA > CountEncoderB + 5) {
      ledcWrite(pwmChannelA, BaseSpeed - 10);
      ledcWrite(pwmChannelB, BaseSpeed + 10);
    } else if (CountEncoderB > CountEncoderA + 5) {
      ledcWrite(pwmChannelA, BaseSpeed + 10);
      ledcWrite(pwmChannelB, BaseSpeed - 10);
    } else {
      ledcWrite(pwmChannelA, BaseSpeed);
      ledcWrite(pwmChannelB, BaseSpeed);
    }
  } else if (!Turning) {
    digitalWrite(Motor[0], LOW);
    digitalWrite(Motor[1], LOW);
    digitalWrite(Motor[2], LOW);
    digitalWrite(Motor[3], LOW);

    ledcWrite(pwmChannelA, 0);
    ledcWrite(pwmChannelB, 0);
    Turning = true;
    CurrentYaw = 0.0;
  }
}

/*=======================================
     Movimiento Angular del Motor
  ========================================*/
void AngularMotor(float TargetAngle, int BaseSpeed) {
  static unsigned long lastUpdate = millis();
  if (Turning) {
    unsigned long now = millis();
    float dt = (now - lastUpdate) / 1000.0;
    lastUpdate = now;

    mpu.dmp_read_fifo();
    float gyroZ = mpu.gyro[2] / 131.0;
    CurrentYaw += gyroZ * dt;
 

    if (abs(CurrentYaw) < TargetAngle) {
      digitalWrite(Motor[0], HIGH);
      digitalWrite(Motor[1], LOW);
      digitalWrite(Motor[2], LOW);
      digitalWrite(Motor[3], HIGH);

      ledcWrite(pwmChannelA, BaseSpeed);
      ledcWrite(pwmChannelB, BaseSpeed);
    } else {
      digitalWrite(Motor[0], LOW);
      digitalWrite(Motor[1], LOW);
      digitalWrite(Motor[2], LOW);
      digitalWrite(Motor[3], LOW);

      ledcWrite(pwmChannelA, 0);
      ledcWrite(pwmChannelB, 0);
      Turning = false;
      Serial.println(CurrentYaw);
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
}