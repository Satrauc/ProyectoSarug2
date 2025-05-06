int Motor[] = {14, 12, 25, 26};
int Pwm[] = {13, 27};
int PinEncoder[] = {34, 35};


void setup() {
  Serial.begin(115200);

  ConfigMotor(Motor, Pwm, 15);
  Encoder(PinEncoder, 330, 6.2);
  //inicializarTransformacion(45.0, 5.0, 5.0); // Matriz de transformacion valores iniciales
  //PrintMatriz();
}

void loop() {

  //DataReception();
  //String MatrizPoint[3];
  //PrintMatriz(MatrizPoint); // Puntos enviados a transformar
  //LinealMotor(50, 115);
  AngularMotor(90, 255);
  //RmpDistance();
  delay(10);

}

/*
  void setup() {
  Serial.begin(115200);
  ConfigMotor(PinMotor, PinPwm, 25);
  Encoder(PinEncoder, 320, 21); // 320 pulsos por vuelta, 21mm diámetro
}
void loop() {
  RmpDistance();
  LinealMotor(50, 225);      // avanzar 50 cm
  AngularMotor(90.0, 225);   // girar 90 grados
}
 */
