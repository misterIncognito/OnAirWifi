#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include <FS.h>  // For SPIFFS

#include "wifi.h"
#include "rest_api.h"
#include "pin_control.h"
#include "led_control.h"

// Default AP (Access Point) credentials
const char* ap_ssid = "On-AirLEDWiFi";
const char* ap_password = "12345678";  // Minimum 8 characters

String ssid = "x";         // Default Wi-Fi credentials changed to "x"
String password = "x";     // Default Wi-Fi credentials changed to "x"

const int ledPin = LED_BUILTIN;  // Built-in LED pin (pin 2 for many boards)


ESP8266WebServer server(80);  // Create an instance of the server, listening on port 80

unsigned long previousMillis = 0;  // For LED blink timing
const long interval = 500;         // Interval for LED blink (500 ms)

bool ledState = LOW;  // To keep track of LED state during blink

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system.");
    return;
  }

  // Load saved Wi-Fi credentials from SPIFFS
  loadWiFiCredentials();

  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Initially turn off the LED

  

  // Check if the module should run in AP mode or connect to a Wi-Fi network
  if (ssid == "x" || password == "x") {
    // Start ESP as an Access Point
    WiFi.softAP(ap_ssid, ap_password);  // Start Access Point with specified SSID and password
    Serial.println("AP Mode: Waiting for configuration...");

    // Print the IP address of the Access Point
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Blink LED while in AP mode
    blinkLED();
  } else {
    // Attempt to connect to the Wi-Fi network with stored credentials
    connectToWiFi(ssid.c_str(), password.c_str());
    // Turn the LED on when connected to Wi-Fi
    digitalWrite(ledPin, HIGH);
  }

  // Setup REST API routes
  setupRestApi();
  
  // Start the web server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Keep blinking the LED if in AP mode
  if (WiFi.status() == WL_IDLE_STATUS) {
    blinkLED();
  }
}



// Handler for the "/wifi" GET endpoint (get current Wi-Fi credentials)
void handleGetWiFi() {
  JSONVar responseDoc;
  responseDoc["ssid"] = ssid;
  responseDoc["password"] = password;

  String responseBody = JSON.stringify(responseDoc);

  server.send(200, "application/json", responseBody);
}

}



