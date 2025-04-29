#include <math.h>

const int Size = 3;
float SMatriz[Size][Size];
float Mlocal[Size] = {1.0, 2.0, 3.0}; // Puntos de entrada de la matriz
float ResultMatriz[Size];
bool Config_ = false;
//int DataLength;
String Data = "";

void setup(){
  Serial.begin(9600);
  //Serial.println("Ingrese el valor del angulo de transformacion");

} void loop(){
  while (true){
    if (Serial.available()){
      char DataInput = Serial.read();
      if(DataInput != ';'){
         Serial.println(DataInput);
         Data += DataInput;
    }else if (Data == '&'){
      Serial.println("pase a aqui");
      for (int i = 0; i = Data.length(); i++){
        Mlocal[i] = Data.toFloat();
        Serial.println(Mlocal[i]);
        delay(500);
      }
    }
  }
}
}void ConvertToFloat(){

}
