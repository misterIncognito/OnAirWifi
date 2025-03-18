#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include "rest_api.h"

#include "led_control.h"
#include "pin_control.h"

#include "rest_wifi.h"
#include "rest_pin.h"
#include "rest_led.h"

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
  sendResponse(200, responseBody);
}

// Helper function to generate a response
String sendResponse(int responseCode, JSONVar responseDoc) {
    String responseBody = JSON.stringify(responseDoc);
    sendResponse(responseCode, responseBody);
    return responseBody;
  }
  
  // Helper function to handle invalid request body errors
  String sendInvalidRequestBodyResponse() {
    JSONVar errorResponse;
    errorResponse["error"] = "Invalid request body";
    return sendResponse(400, errorResponse);  // Bad Request
  }
  
  // Helper function to handle invalid JSON errors
  String sendInvalidJsonResponse() {
    JSONVar errorResponse;
    errorResponse["error"] = "Invalid JSON";
    return sendResponse(400, errorResponse);  // Bad Request
  }