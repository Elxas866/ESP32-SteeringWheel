#include <Arduino.h>
#include <BleGamepad.h>

void updateEncoder();
void initBleGamepad();
void quickTest();

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
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  // // Interrupthandler für die Encoder-Pins konfigurieren
  // attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);
}

void loop() {
  updateEncoder();

  if (bleGamepad.isConnected()) {
    //quickTest();
    if (encoderPos> -32767 && encoderPos < 32767) {
      bleGamepad.setSteering(encoderPos);
      bleGamepad.sendReport();
    }
  }
}

void updateEncoder() {
  int MSB = digitalRead(encoderPinA);
  int LSB = digitalRead(encoderPinB);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderPos += 256;
  }
  else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderPos -= 256;
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

void quickTest() {
  Serial.println("Press all buttons one by one");
  for (int i = 1; i <= numOfButtons; i += 1)
  {
      bleGamepad.press(i);
      bleGamepad.sendReport();
      delay(100);
      bleGamepad.release(i);
      bleGamepad.sendReport();
      delay(25);
  }

  Serial.println("Move steering from center to max");
  for (int i = 0; i > -32767; i -= 256)
  {
      bleGamepad.setSteering(i);
      bleGamepad.sendReport();
      delay(10);
  }

  Serial.println("Move steering from min to max");
  for (int i = -32767; i < 32767; i += 256)
  {
      bleGamepad.setSteering(i);
      bleGamepad.sendReport();
      delay(10);
  }

  Serial.println("Move steering from max to center");
  for (int i = 32767; i > 0; i -= 256)
  {
      bleGamepad.setSteering(i);
      bleGamepad.sendReport();
      delay(10);
  }
  bleGamepad.setSteering(0);
  bleGamepad.sendReport();

  Serial.println("Move accelerator from min to max");
  // for(int i = 32767 ; i > -32767 ; i -= 256)    //Use this for loop setup instead if accelerator is reversed
  for (int i = -32767; i < 32767; i += 256)
  {
      bleGamepad.setAccelerator(i);
      bleGamepad.sendReport();
      delay(10);
  }
  bleGamepad.setAccelerator(-32767);
  bleGamepad.sendReport();

  Serial.println("Move brake from min to max");
  for (int i = -32767; i < 32767; i += 256)
  {
      bleGamepad.setBrake(i);
      bleGamepad.sendReport();
      delay(10);
  }
  bleGamepad.setBrake(-32767);
  bleGamepad.sendReport();
}