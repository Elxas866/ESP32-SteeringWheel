#include <Arduino.h>
#include <BleMouse.h>

void updateEncoder();
void moveMouse(unsigned long);

// Pins für den Encoder
const int encoderPinA = 12;
const int encoderPinB = 14;

// Variablen zur Speicherung des Zustands des Encoders
volatile int encoderPos = 0;
volatile int lastEncoded = 0;
volatile long lastencoderValue = 0;

BleMouse bleMouse("ESP32 Steering Wheel", "Elxas866", 100);

void setup() {
  Serial.begin(9600);

  Serial.println("Starting BLE Mouse...");
  bleMouse.begin();
  Serial.println("BLE Mouse started!");

  // Encoder-Pins als Eingänge konfigurieren
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  // Interrupthandler für die Encoder-Pins konfigurieren
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);

}

void loop() {

  if (bleMouse.isConnected()) {
    bleMouse.move(encoderPos * 10, 0, 0);
    Serial.println("Moved.");
    Serial.println("X: " + String(encoderPos * 10));
  }
  delay(1000);

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