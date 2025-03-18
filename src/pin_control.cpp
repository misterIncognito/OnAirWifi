#include <Arduino_JSON.h>
#include "pin_control.h"

// Define input pins
const int inputPin1 = D1; // GPIO 5 (D1 on many boards)
const int inputPin2 = D2; // GPIO 4 (D2 on many boards)

// Define output pins
const int outputPin1 = D6; // GPIO 12 (D6 on many boards)
const int outputPin2 = D7; // GPIO 13 (D7 on many boards)


extern ESP8266WebServer server;

// Setup pin modes and initial states
void setupPins() {
  // Set input pins as inputs
  pinMode(inputPin1, INPUT);
  pinMode(inputPin2, INPUT);

  // Set output pins as outputs
  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);

  // Set initial states of output pins
  digitalWrite(outputPin1, LOW);
  digitalWrite(outputPin2, LOW);
}

// Set pin mode (input/output)
void setPinMode(int pin, int mode) {
  pinMode(pin, mode);
}


// Set pin mode (input/output)
void setPinState(int pin, int state) {
    digitalWrite(outputPin1, state ? HIGH : LOW);
}

