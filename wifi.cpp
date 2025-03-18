#include <ESP8266WiFi.h>
#include <FS.h>
#include <Arduino_JSON.h>
#include "wifi.h"

void connectToWiFi(const char* ssid, const char* password) {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loadWiFiCredentials() {
  File file = SPIFFS.open("/wifi_credentials.json", "r");
  if (file) {
    String fileContent = file.readString();
    JSONVar json = JSON.parse(fileContent);
    if (JSON.typeof(json) != "undefined") {
      ssid = String(json["ssid"]);
      password = String(json["password"]);
    }
    file.close();
  }
}

void saveWiFiCredentials(String ssid, String password) {
  File file = SPIFFS.open("/wifi_credentials.json", "w");
  if (file) {
    JSONVar json;
    json["ssid"] = ssid;
    json["password"] = password;
    String jsonString = JSON.stringify(json);
    file.print(jsonString);
    file.close();
  }
}
