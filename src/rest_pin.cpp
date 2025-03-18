#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include "rest_pin.h"
#include "pin_control.h"


extern ESP8266WebServer server;
// Handle endpoint for getting pin states
void handleGetPins() {
    JSONVar responseDoc;
    
    // Reading input and output pin states
    responseDoc["inputPin1"] = digitalRead(inputPin1);
    responseDoc["inputPin2"] = digitalRead(inputPin2);
    responseDoc["outputPin1"] = digitalRead(outputPin1);
    responseDoc["outputPin2"] = digitalRead(outputPin2);
  
    String responseBody = JSON.stringify(responseDoc);
    sendResponse(200, responseBody);
  }
  
  // Handler for the "/pin/out" GET endpoint (get output pin states)
  void handleGetOutputPins() {
      JSONVar responseDoc;
      
      responseDoc["outputPin1"] = digitalRead(outputPin1);
      responseDoc["outputPin2"] = digitalRead(outputPin2);
    
      String responseBody = JSON.stringify(responseDoc);
      sendResponse(200, responseBody);
    }
  

  
  // Handler for the "/pin/output" POST endpoint (set a specific output pin state)
  
  // Handle endpoint for setting pin state
  void handleSetOutputPins() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      JSONVar json = JSON.parse(body);
      
      if (JSON.typeof(json) == "undefined") {
        sendInvalidJsonResponse();
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
        JSONVar errorResponse;
        errorResponse["error"] = "Invalid pin number";
        sendResponse(400, errorResponse);
        return;
      }
  
      // Create response JSON
      JSONVar responseDoc;
      responseDoc["status"] = "Output Pin state updated";
      responseDoc["pin"] = pin;
      responseDoc["state"] = state;
  
      String responseBody = JSON.stringify(responseDoc);
      sendResponse(200, responseBody);
    } else {
      sendInvalidJsonResponse();
    }
  }
  