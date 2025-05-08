void setup() {
    Serial.begin(115200);
}

void loop() {
    //funcion_modular();  // Llamada a la función que está en otro archivo
    delay(1000);
}
/******************************************************************************/
//Ejemplo QRE1114
/******************************************************************************/
/**
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
/******************************************************************************/
//Ejemplo Comunicación
/******************************************************************************/
/**
void setup() {
  Serial.begin(115200);
  setupBLE();
}

void loop() {
  String Data = readDataBLE();
  if(Data!=""){
    parsearDatos(Data);
    Serial.println(Data);
    imprimirDatos() ;
  }
  //sendDataBLE("Hola desde ESP32");
  delay(1000);
}

/**/
/**/
/******************************************************************************/
//Ejemplo MPU6500
/******************************************************************************/
/**

/**/
//*****************************************************************
// Ejemplo Motores
//*****************************************************************
/**

}
/**/
