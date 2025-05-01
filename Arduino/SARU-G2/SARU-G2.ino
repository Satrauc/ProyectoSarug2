void setup() {
    Serial.begin(9600);
}

void loop() {
    //funcion_modular();  // Llamada a la función que está en otro archivo
    delay(1000);
}
/******************************************************************************/
//Ejemplo QRE1114
/******************************************************************************/
/**
const int sensores[] = {A0, A1, A2};  // Usa de 1 a 4 sensores
const int cantidadSensores = 3;
const int umbral = 500;

void setup() {
  Serial.begin(9600);
  configurarSensores(sensores, cantidadSensores);
}

void loop() {
  bool objeto = detectarObjetoRedundante(umbral, sensores, cantidadSensores);
  Serial.println(objeto ? "Objeto detectado" : "Sin objeto");
  delay(200);
}
/**/
