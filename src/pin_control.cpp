#include <ESP8266WebServer.h>
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

// Handle endpoint for getting pin states
void handleGetPins() {
  JSONVar responseDoc;
  
  // Reading input and output pin states
  responseDoc["inputPin1"] = digitalRead(inputPin1);
  responseDoc["inputPin2"] = digitalRead(inputPin2);
  responseDoc["outputPin1"] = digitalRead(outputPin1);
  responseDoc["outputPin2"] = digitalRead(outputPin2);

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

// Handler for the "/pin/out" GET endpoint (get output pin states)
void handleGetOutputPins() {
    JSONVar responseDoc;
    
    responseDoc["outputPin1"] = digitalRead(outputPin1);
    responseDoc["outputPin2"] = digitalRead(outputPin2);
  
    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  }

  // Handler for the "/pin/input" GET endpoint (get input pin states)
void handleGetInputPins() {
    JSONVar responseDoc;
    
    responseDoc["inputPin1"] = digitalRead(inputPin1);
    responseDoc["inputPin2"] = digitalRead(inputPin2);
  
    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  }

  // Handler for the "/pin/output" POST endpoint (set a specific output pin state)

// Handle endpoint for setting pin state
void handleSetOutputPins() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);
    
    if (JSON.typeof(json) == "undefined") {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    int pin = json["pin"];
    int state = json["state"];
    
    // Set pin state (HIGH/LOW)
    if (pin == 1) {
        setPinState(outputPin1, state);
    } else if (pin == 2) {
        setPinState(outputPin2, state);
    } else {
      server.send(400, "application/json", "{\"error\":\"Invalid pin number\"}");
      return;
    }

    // Create response JSON
    JSONVar responseDoc;
    responseDoc["status"] = "Output Pin state updated";
    responseDoc["pin"] = pin;
    responseDoc["state"] = state;

    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
  }
}
