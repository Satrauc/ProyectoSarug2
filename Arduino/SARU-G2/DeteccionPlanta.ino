// COnfiguración del pin
void ConfigQRE1113(int QRE_PIN){
  pinMode(QRE_PIN, INPUT);

}

// Función para detectar un objeto con el sensor QRE1113
bool detectarObjeto(int UMBRAL, int QRE_PIN) {
  int lectura = analogRead(QRE_PIN);  // Leemos el valor del sensor (0-1023)
  
  // Si la lectura es menor que el umbral, significa que el sensor detectó un objeto
  if (lectura < UMBRAL) {
    return true;  // Objeto detectado
  } else {
    return false; // No se detectó objeto
  }
}