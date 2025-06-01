/*=======================================
  Autor: Isabella Benavides Cifuentes
  ========================================*/

//#include <Simple_MPU6050.h> // https://github.com/ZHomeSlice/Simple_MPU6050
//#include <Simple_Wire.h>    
//#include <Wire.h>
//int Motor[4];
//int Pwm[2]; 

/*=======================================
           Pines del Motor TB6612
  ========================================*/
int Motor[] = {14, 12, 25, 26};           // Pin Motores
int Pwm[] = {13, 27};                     // Pin PWM
int Stby = 15;

const int pwmChannelA = 0;
const int pwmChannelB = 1;
int BaseSpeed = 100;

/*=======================================
          Variables Encoder
  ========================================*/
int PinEncoder[] = {34, 35};                // Pin Encoder

int EncoderFullTurn;
volatile int CountEncoderA = 0, CountEncoderB = 0;
volatile int DistanceEncoder = 0;
unsigned long FormerTime = 0;
float Rpm = 0.0;

/*=======================================
     Acelerómetro y Giroscopio MPU6050
  ========================================*/
//Simple_MPU6050 mpu;
float CurrentYaw = 0.0;

/*=======================================
          Parámetros necesarios
  ========================================*/
bool Turning = false;
float Distance = 0.0;
int Perimeter;

float errorAnguloMax = 2.0;       // Margen de error en grados para el giro
float errorDistanciaMax = 1.0;    // Margen de error en cm para avanzar
float distanciaSensorALlanta = 10.0; // cm, ajustar según la posición del sensor
/*=======================================
       Configuración de los Motores
  ========================================*/
void ConfigMotor( ) {
  for (byte i = 0; i < 4; i++) pinMode(Motor[i], OUTPUT);
  pinMode(Stby, OUTPUT);
  digitalWrite(Stby, HIGH);
  
  ledcSetup(pwmChannelA, 1000, 8);
  ledcAttachPin(Pwm[0], pwmChannelA);
  ledcSetup(pwmChannelB, 1000, 8);
  ledcAttachPin(Pwm[1], pwmChannelB);
  
  digitalWrite(Stby, HIGH);
}

/*=======================================
       Configuración de los Encoders
  ========================================*/
void ConfigEncoder(int encoderTurnCount, float Diameter) {
  EncoderFullTurn = encoderTurnCount;
  Perimeter = PI * Diameter;
  for (byte i = 0; i < 2; i++) pinMode(PinEncoder[i], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinEncoder[0]), UpdateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(PinEncoder[1]), UpdateEncoderB, RISING);
}

/*=======================================
         Cálculo RPM & Distancia
  ========================================*/
float RmpDistance() {
  unsigned long CurrentTime = millis();
  if (CurrentTime - FormerTime >= 100) {
    noInterrupts();
    int PulseRead = CountEncoderA;
    int PulseDistance = DistanceEncoder;
    CountEncoderA = 0;
    interrupts();
    Rpm = (PulseRead / (float)EncoderFullTurn) * 60.0;
    Distance = (PulseDistance / (float)EncoderFullTurn) * Perimeter;
    FormerTime = CurrentTime;
    
    return Rpm;
  }
  return 0;
}

/*=======================================
       Movimiento Lineal del Motor
  ========================================*/
void LinealMotor(float DistancePoint, int BaseSpeed, bool reverse) {
  
  if (Distance <= DistancePoint && !Turning) {
    if(reverse){
      digitalWrite(Motor[0], HIGH);
      digitalWrite(Motor[1], LOW);
      digitalWrite(Motor[2], HIGH);
      digitalWrite(Motor[3], LOW);
    }else{
      digitalWrite(Motor[0], LOW);
      digitalWrite(Motor[1], HIGH);
      digitalWrite(Motor[2], LOW);
      digitalWrite(Motor[3], HIGH);
    }

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

    //mpu.dmp_read_fifo();
    //float gyroZ = mpu.gyro[2] / 131.0;
    actualizarValores();
    float angulo = obtenerAnguloZ();
    CurrentYaw += angulo * dt;

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

/*=======================================
     Funciones Cuartas
  ========================================*/
/**
void EjecutarMovimientoLineal(float distancia, int velocidad, bool reversa) {
  Distance = 0;
  Turning = false;

  while (!Turning) {
    LinealMotor(distancia, velocidad, reversa);
    delay(10);  // Pequeña pausa para evitar sobrecargar el CPU
  }
}
void EjecutarMovimientoAngular(float angulo, int velocidad) {
  Turning = true;
  CurrentYaw = 0.0;

  while (Turning) {
    AngularMotor(angulo, velocidad);
    delay(10);
  }
}
/**/

void Mover(float distanciaObjetivo, bool reversa) {
  float Proporcional = 0.65;
  int velocidadBase = VELOCIDAD_LINEAL;
  Distance = 0;  // Reiniciar distancia
  bool completado = false;
  sendDataBLE("----Distancia Objetivo "+String(distanciaObjetivo));
  Serial.println("----Distancia Objetivo "+String(distanciaObjetivo));
  Actualizar_EnvioDatos();
  Resetang();
  float anguloInicial = obtenerAnguloZ();
  
  while (!completado) {
    
    Actualizar_EnvioDatos();
    
    sendDataBLE("Distancia actual: "+String(Distance)+" Error: "+String(Distance - distanciaObjetivo));
    Serial.println("Distancia actual: "+String(Distance)+" Error: "+String(Distance - distanciaObjetivo));
    if (Distance >= distanciaObjetivo) {
      MoverMotores(0, 0);
      break;
    }

    // Obtener error de desviación lateral
    float anguloActual = obtenerAnguloZ();
    float errorA = anguloActual - anguloInicial;

    int velocidadIzq = 0;
    int velocidadDer = 0;
    
    // Si el error lateral es grande, detener y corregir
    if (abs(errorA) > 0.2) { // Umbral en metros (50 cm)
      //MoverMotores(0, 0);  // Detenerse antes de corregir
      //delay(200);
      //Serial.println("Error: "+String(errorA));
      if (errorA > 0) {
        // Se desvió hacia la Derecha → girar a la Izquierda
        
        // bajar proporcional a la Izquierda
        velocidadIzq = reversa ? (int)(velocidadBase * Proporcional) : (int)(-velocidadBase * Proporcional);
        velocidadDer = reversa ? velocidadBase : -velocidadBase;
        
      } else if(errorA < 0){
        // Se desvió hacia la Izquierda → girar a la Derecha
        
        // bajar proporcional a la Derecha
        velocidadIzq = reversa ? velocidadBase : -velocidadBase;
        velocidadDer = reversa ? (int)(velocidadBase * Proporcional) : (int)(-velocidadBase * Proporcional);
        
      }
    }else{
      // Avanzar recto
      velocidadIzq = reversa ? velocidadBase : -velocidadBase;
      velocidadDer = reversa ? velocidadBase : -velocidadBase;
    }
    //Serial.println("Derecha:"+ String(velocidadDer));
    //Serial.println("Izquieda:"+ String(velocidadIzq));
    

    MoverMotores(velocidadDer, velocidadIzq);
    delay(10);
  }
}
void Girar(float anguloObjetivo) {
  sendDataBLE("----Angulo Objetivo "+String(anguloObjetivo));
  Serial.println("----Angulo Objetivo "+String(anguloObjetivo));
  if(anguloObjetivo < 0){     // Si el angulo al que se quiere llegar, es negativo, se gira a la Derecha (-1 a -179)

    GirarDerecha(anguloObjetivo);

  }else if(anguloObjetivo>0){ // Si el angulo al que se quiere llegar, es positivo, se gira a la Izquierda (1 a 179)

    GirarIzquierda(anguloObjetivo);
    
  }

}

void GirarDerecha(float gradosObjetivo) {
  int velocidad = VELOCIDAD_ANGULAR;
  float margenError = ERROR_ANGULAR;

  if(fabs(gradosObjetivo) >= 90 ){
    ResetEncoder();
    Mover(10,false);
  }

  Actualizar_EnvioDatos();  
  Resetang();
  delay(50); 

  Actualizar_EnvioDatos();
  float anguloInicial = obtenerAnguloZ();
  float anguloFinal = anguloInicial + gradosObjetivo;

  // Normaliza a -180 a 180
  if (anguloFinal > 180) anguloFinal -= 360;
  if (anguloFinal < -180) anguloFinal += 360;

  float error = 0;
  bool doblegiro = false; 
  float anguloActual = 0;
  do {
    Actualizar_EnvioDatos();
    if(fabs(anguloActual) > 90 && !doblegiro){ // Angulos mayores de 90 ajusta la posicion del robot
      MoverMotores(0, 0);
      ResetEncoder();
      delay(20);
      if(fabs(gradosObjetivo) <= 135){
        Mover(10,false);
      }else{
        Mover(15,false);
      }
      delay(20);
      doblegiro = true;
    }
    anguloActual= obtenerAnguloZ();
    
    // Calcular error y normalizar
    error = anguloFinal - anguloActual;
    while (error > 180) error -= 360;
    while (error < -180) error += 360;

    // Girar suavemente hacia la derecha (solo rueda izquierda)
    MoverMotores(0, velocidad);
    delay(30);
    MoverMotores(0, 0);
    delay(20);
    sendDataBLE("Angulo actual: "+String(anguloActual)+" Error: "+String(error));
    Serial.println("Angulo actual: "+String(anguloActual)+" Error: "+String(error));
  } while (fabs(error) > margenError);// mientras el error sea mayor
  if(fabs(gradosObjetivo) <= 90){// Angulos mayores de 90 ajusta la posicion del robot
    MoverMotores(0, 0);
    ResetEncoder();
    Mover(5,false);
    delay(20);
  }
  // Detener por seguridad
  MoverMotores(0, 0);
}
void GirarIzquierda(float gradosObjetivo) {
  int velocidad = VELOCIDAD_ANGULAR;
  float margenError = ERROR_ANGULAR;

  if(fabs(gradosObjetivo) >= 90 ){
    
    ResetEncoder();
    Mover(10,false);
  }
  Actualizar_EnvioDatos();  
  Resetang();
  delay(50);                // Espera breve para estabilizar

  Actualizar_EnvioDatos();  // Vuelve a actualizar antes de iniciar giro
  float anguloInicial = obtenerAnguloZ();
  float anguloFinal = anguloInicial + gradosObjetivo;

  // Normaliza a -180 a 180
  if (anguloFinal > 180) anguloFinal -= 360;
  if (anguloFinal < -180) anguloFinal += 360;

  float error = 0;
  bool doblegiro = false; 
  float anguloActual = 0;

  do {
    Actualizar_EnvioDatos();
    if(fabs(anguloActual) > 90 && !doblegiro){ // Angulos mayores de 90 ajusta la posicion del robot
      MoverMotores(0, 0);
      ResetEncoder();
      delay(20);
      if(fabs(gradosObjetivo) <= 135){
        Mover(10,false);
      }else{
        Mover(15,false);
      }
      delay(20);
      doblegiro = true;
    }
    anguloActual = obtenerAnguloZ();
    
    // Calcular error y normalizar
    error = anguloFinal - anguloActual;
    while (error > 180) error -= 360;
    while (error < -180) error += 360;

    // Girar suavemente hacia la izquierda (solo rueda derecha)
    MoverMotores(velocidad, 0);
    delay(30);
    MoverMotores(0, 0);
    delay(20);
    sendDataBLE("Angulo actual: "+String(anguloActual)+" Error: "+String(error));
    Serial.println("Angulo actual: "+String(anguloActual)+" Error: "+String(error));
  } while (fabs(error) > margenError);// mientras el error sea mayor
  
  if(fabs(gradosObjetivo) <= 90){
    MoverMotores(0, 0);
    ResetEncoder();
    Mover(5,false);
    delay(20);
  }
  // Detener por seguridad
  MoverMotores(0, 0);
}
/**/
void MoverMotores(int velocidadDer, int velocidadIzq) {
  // Motor A (izquierdo)
  digitalWrite(Motor[0], velocidadIzq >= 0);
  digitalWrite(Motor[1], velocidadIzq < 0);
  ledcWrite(pwmChannelA, abs(velocidadIzq));

  // Motor B (derecho)
  digitalWrite(Motor[2], velocidadDer >= 0);
  digitalWrite(Motor[3], velocidadDer < 0);
  ledcWrite(pwmChannelB, abs(velocidadDer));
  
  UpdateEncoderA();       // Encoder
  UpdateEncoderB();       // Encoder
}
void ResetEncoder() {
  noInterrupts();
  CountEncoderA = 0;
  CountEncoderB = 0;
  DistanceEncoder = 0;
  Distance = 0;
  FormerTime = millis();
  interrupts();
}
