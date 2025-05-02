int MotorPins[] = {1, 2, 3, 4};
int Pwm[] = {2, 3};
int EncoderPins[] = {5, 6};

void setup() {
  
  ConfigMotor(MotorPins, Pwm, 6); //(Motores, Pwm, Stby)
  Encoder(EncoderPins, 1234, 5); //(EncoderPin, EncoderFullTurn, Diamter)
  /*=======================
       Manual de uso:
  =======================*/
  /*=======================
          Motores
  ========================*/
  //  ConfigMotor Realiza la cofiguración inicial de los motores.
  //  MotorPins: Establecer los pines fisicos del tb6612fng.
  //  Pwm: Establece los pines que cuenten con PWM.
  //  Stby: Pin que permite inicializar el Driver.
  
  /*=======================
          Encoder
  ========================*/
  //  EncoderPins: Pines A y B del encoder fisicos.
  //  EncoderFullturn: Este parametro lo dara el Datasheed es cuantos pulsos completa una vuelta.
  //  Diamter: Permite realizar el calculo de la velocidad lineal.

}
void loop() {
  /*==========================
   LinealMotor & AngularMotor
  ===========================*/
  //  LinealMotor (TargetDistance, BaseSpeed)
  //  AngularMotor (TargetAngle, BaseSpeed)
  //  BaseSpeed permite avanzar o girar a la velocidad que deseemos.
  LinealMotor(40, 255);
  AngularMotor(90, 255);

}
