#include "robot.h"

void setup() {
  Serial.begin(9600);
  iniciarMotores();
  iniciarEncoder();
  iniciarMPU();
}

void loop() {
  calcularRPMyDistancia();
  moverse();
}
