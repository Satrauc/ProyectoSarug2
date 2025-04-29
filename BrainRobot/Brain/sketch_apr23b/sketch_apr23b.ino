#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

// Pines
const uint8_t PULSADOR1 = 2;
const uint8_t PULSADOR2 = 3;
const uint8_t LED1      = 10;
const uint8_t LED2      = 11;

// SoftSerial para DFPlayer
SoftwareSerial dfSerial(10, 11); // TX=10 → RX DP, RX=11 ← TX DP
DFRobotDFPlayerMini dfplayer;

// Estados
bool estado1 = false;
bool estado2 = false;

// Variables de parpadeo
unsigned long prevMillis1 = 0;
unsigned long prevMillis2 = 0;
const unsigned long intervalo = 300; // ms

void setup() {
  pinMode(PULSADOR1, INPUT_PULLUP);
  pinMode(PULSADOR2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Serial.begin(9600);
  dfSerial.begin(9600);
  if (!dfplayer.begin(dfSerial)) {
    Serial.println("DFPlayer init error");
    while (true);
  }
  dfplayer.volume(20);          // 0–30
  dfplayer.enableLoopAll();     // opcional
}

void loop() {
  // Lectura de pulsadores (LOW = presionado)
  if (digitalRead(PULSADOR1) == LOW) {
    estado1 = true;
    estado2 = false;
    dfplayer.play(1);  // reproduzco 001.mp3
    delay(200);
  }
  if (digitalRead(PULSADOR2) == LOW) {
    estado2 = true;
    estado1 = false;
    dfplayer.play(2);  // reproduzco 002.mp3
    delay(200);
  }

  unsigned long now = millis();
  // Parpadeo LED1
  if (estado1) {
    if (now - prevMillis1 >= intervalo) {
      prevMillis1 = now;
      digitalWrite(LED1, !digitalRead(LED1));
    }
  } else digitalWrite(LED1, LOW);

  // Parpadeo LED2
  if (estado2) {
    if (now - prevMillis2 >= intervalo) {
      prevMillis2 = now;
      digitalWrite(LED2, !digitalRead(LED2));
    }
  } else digitalWrite(LED2, LOW);
}
