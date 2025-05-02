int MotorPins[] = {1, 2, 3, 4};
int Pwm[] = {2, 3};
int EncoderPins[] = {5, 6};


void setup() {
  // Manual de uso
  ConfigMotor(MotorPins, Pwm, 6); //(Motores, Pwm, BaseSpeed, Stby)
  Encoder(EncoderPins, 2434, 5); //(EncoderPin, EncoderFullTurn, Diamter)
}
void loop() {
  LinealMotor(40, 255); // (Distance, BaseSpeed)
  AngularMotor(90, 255); // (Angle, BaseSpeed)

}
