#include <Arduino.h>
#include <BleGamepad.h>

void updateEncoder();
void initBleGamepad();

BleGamepad bleGamepad("ESP32 steering wheel", "Elxas866", 100);

// gamepad definitions
#define numOfButtons 10
#define numOfHatSwitches 0
#define enableX false
#define enableY false
#define enableZ false
#define enableRX false
#define enableRY false
#define enableRZ false
#define enableSlider1 false
#define enableSlider2 false
#define enableRudder false
#define enableThrottle false
#define enableAccelerator true
#define enableBrake true
#define enableSteering true

// Pins für den Encoder
const int encoderPinA = 12;
const int encoderPinB = 14;

// Variablen zur Speicherung des Zustands des Encoders
volatile int encoderPos = 0;
volatile int lastEncoded = 0;
volatile long lastencoderValue = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("Initializing BLE Gamepad...");
  initBleGamepad();
  Serial.println("BLE Gamepad initialized!");

  // Encoder-Pins als Eingänge konfigurieren
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  // Interrupthandler für die Encoder-Pins konfigurieren
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);

}

void loop() {
  if (bleGamepad.isConnected()) {
    bleGamepad.setAxes();
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

void initBleGamepad() {
  // Setup controller with 10 buttons, accelerator, brake and steering
    BleGamepadConfiguration bleGamepadConfig;
    bleGamepadConfig.setAutoReport(false);
    bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
    bleGamepadConfig.setButtonCount(numOfButtons);
    bleGamepadConfig.setWhichAxes(enableX, enableY, enableZ, enableRX, enableRY, enableRZ, enableSlider1, enableSlider2);      // Can also be done per-axis individually. All are true by default
    bleGamepadConfig.setWhichSimulationControls(enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering); // Can also be done per-control individually. All are false by default
    bleGamepadConfig.setHatSwitchCount(numOfHatSwitches);                                                                      // 1 by default
    // Some non-Windows operating systems and web based gamepad testers don't like min axis set below 0, so 0 is set by default
    bleGamepadConfig.setSimulationMin(0x8001); // -32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
    bleGamepadConfig.setSimulationMax(0x7FFF); //32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal 
	  bleGamepad.begin(&bleGamepadConfig);
    // changing bleGamepadConfig after the begin function has no effect, unless you call the begin function again

    // Set accelerator and brake to min
    bleGamepad.setAccelerator(-32767);
    bleGamepad.setBrake(-32767);

    // Set steering to center
    bleGamepad.setSteering(0);
}