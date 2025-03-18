#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "rest_wifi.h"
#include "wifi.h"

void handleGetWiFi() {
  JSONVar responseDoc;
  responseDoc["ssid"] = ssid;
  responseDoc["password"] = password;

  String responseBody = JSON.stringify(responseDoc);
  sendResponse(200, responseBody);
}

void handleSetWiFi() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);

    if (JSON.typeof(json) == "undefined") {
      sendInvalidJsonResponse();
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
    sendResponse(200, responseBody);
  } else {
    sendInvalidRequestBodyResponse();
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
  sendResponse(200, responseBody);
}
