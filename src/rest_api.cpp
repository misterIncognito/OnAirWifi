#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include "rest_api.h"
#include "wifi.h"
#include "led_control.h"
#include "pin_control.h"

extern ESP8266WebServer server;

void setupRestApi() {
  server.on("/wifi", HTTP_GET, handleGetWiFi);
  server.on("/wifi", HTTP_POST, handleSetWiFi);
  server.on("/wifi/reset", HTTP_GET, handleResetWiFi);
  server.on("/led", HTTP_GET, handleGetLed);
  server.on("/led", HTTP_POST, handleSetLed);
  server.on("/health", HTTP_GET, handleGetHealth);
  
  // Pin control endpoints
  server.on("/pin/input", HTTP_GET, handleGetInputPins);  // Get input pin states
  server.on("/pin/output", HTTP_GET, handleSetOutputPins);  // Get output pin states
  server.on("/pin/output", HTTP_POST, handleSetOutputPins); // Set output pin states

}

void handleGetHealth() {
  JSONVar responseDoc;
  responseDoc["status"] = (WiFi.status() == WL_CONNECTED) ? "connected" : "disconnected";
  responseDoc["ip"] = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "";

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

void handleGetWiFi() {
  JSONVar responseDoc;
  responseDoc["ssid"] = ssid;
  responseDoc["password"] = password;

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

void handleSetWiFi() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);
    if (JSON.typeof(json) == "undefined") {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    String new_ssid = json["ssid"];
    String new_password = json["password"];

    saveWiFiCredentials(new_ssid, new_password);

    WiFi.disconnect();
    delay(1000);
    WiFi.begin(new_ssid.c_str(), new_password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    JSONVar responseDoc;
    responseDoc["status"] = "Wi-Fi credentials updated";
    responseDoc["ssid"] = new_ssid;
    responseDoc["ip"] = WiFi.localIP().toString();

    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
  }
}

void handleResetWiFi() {
  ssid = "x";
  password = "x";
  saveWiFiCredentials(ssid, password);
  ESP.restart();

  JSONVar responseDoc;
  responseDoc["status"] = "Wi-Fi credentials reset to default";
  responseDoc["ssid"] = ssid;
  responseDoc["password"] = password;

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

void handleGetLed() {
  JSONVar responseDoc;
  responseDoc["led_state"] = (ledState ? "on" : "off");

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

void handleSetLed() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);
    if (JSON.typeof(json) == "undefined") {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    bool led = json["led"];
    digitalWrite(ledPin, led ? HIGH : LOW);

    JSONVar responseDoc;
    responseDoc["status"] = "LED state updated";

    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
  }
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
  