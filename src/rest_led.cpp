#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include "led_control.h"
#include "rest_led.h"
#include "rest_api.h"

extern ESP8266WebServer server;


void handleGetLed() {
  JSONVar responseDoc;
  responseDoc["led_state"] = (ledState ? "on" : "off");
  String responseBody = JSON.stringify(responseDoc);
  sendResponse(200, responseBody);
}

void handleSetLed() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);
    if (JSON.typeof(json) == "undefined") {
      sendInvalidJsonResponse();
      return;
    }

    bool led = json["led"];
    digitalWrite(ledPin, led ? HIGH : LOW);

    JSONVar responseDoc;
    responseDoc["status"] = "LED state updated";

    String responseBody = JSON.stringify(responseDoc);
    sendResponse(200, responseBody);
  } else {
    sendInvalidRequestBodyResponse();
  }
}
