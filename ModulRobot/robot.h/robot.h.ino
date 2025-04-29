#ifndef ROBOT_H
#define ROBOT_H

#include <Simple_MPU6050.h>
#include <Simple_Wire.h>
#include <Wire.h>

// Variables Motor
extern const int Motor[];
extern const int Pwm[];
extern const int Stby;
extern const int BaseSpeed;

// Variables Encoder
extern const int PinA;
extern const int PinB;
extern const int EncoderFullTurn;
extern volatile int CountEncoderA;
extern volatile int CountEncoderB;
extern volatile int DistanceEncoder;
extern unsigned long FormerTime;
extern float Rpm;

// Acelerómetro y Giroscopio
extern Simple_MPU6050 mpu;
extern float CurrentYaw;
extern bool Turning;

// Parámetros
extern const int Perimeter;
extern float Distance;

// Modificar para Probar
extern float DistancePoint;
extern float TargetAngle;

// Funciones
void iniciarMotores();
void iniciarEncoder();
void iniciarMPU();
void calcularRPMyDistancia();
void moverse();
void UpdateEncoderA();
void UpdateEncoderB();

#endif
