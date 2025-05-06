#include <BluetoothSerial.h>

String buffer = "";
BluetoothSerial BT;

void BluetoothInit(){
  Serial.begin(9600);
  BT.begin("Sarug2");
}

void DataReception(){
  while (BT.available()){
    char InData = BT.read();
    if (InData == '@') {
      int Count$ = 0;

      for (int i = 0; i < buffer.length(); i++){
        if (buffer.charAt(i) == '$') {
          Count$++;
        }
      }
      String Matriz[Count$ + 1];
      int lastIndex = 0;
      int nextIndex = 0;

      for (int i = 0; i < Count$; i++){
        nextIndex = buffer.indexOf('$', lastIndex);
        Matriz[i] = buffer.substring(lastIndex, nextIndex);
        lastIndex = nextIndex + 1;
      }
      Matriz[Count$] = buffer.substring(lastIndex);
      /*
        for (int i = 0; i <= Count$; i++) {
         Serial.print("datos[");
         Serial.print(i);
         Serial.print("]: ");
         Serial.println(Matriz[i]);
        }
      */
      buffer = "";
    } else{
      buffer += InData;
    }
  }
}
