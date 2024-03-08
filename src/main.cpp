#include <Arduino.h>

void updateEncoder();

// Pins für den Encoder
const int encoderPinA = 2;
const int encoderPinB = 3;

// Variablen zur Speicherung des Zustands des Encoders
volatile int encoderPos = 0;
volatile int lastEncoded = 0;
volatile long lastencoderValue = 0;

void setup() {
  Serial.begin(9600);

  // Encoder-Pins als Eingänge konfigurieren
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  // Interrupthandler für die Encoder-Pins konfigurieren
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);
}

void loop() {
  // Hier kannst du die Encoder-Position verwenden
  Serial.println(encoderPos); // Über die serielle Schnittstelle ausgeben
  delay(100); // Eine kurze Pause für die Ausgabe
}

void updateEncoder() {
  int MSB = digitalRead(encoderPinA);
  int LSB = digitalRead(encoderPinB);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderPos++;
  }
  else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderPos--;
  }

  lastEncoded = encoded;
}
