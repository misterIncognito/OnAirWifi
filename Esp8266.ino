#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Define the LED pin
const int ledPin = LED_BUILTIN;  // Typically the built-in LED pin (pin 2 for many boards)

ESP8266WebServer server(80);  // Create an instance of the server, listening on port 80

void setup() {
  // Start the serial communication
  Serial.begin(115200);
  
  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Turn off LED initially
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Handle GET requests at the root path ("/status")
  server.on("/status", HTTP_GET, handleStatus);
  
  // Handle GET requests for "/led" to toggle the LED
  server.on("/led", HTTP_GET, handleLedToggle);
  
  // Handle POST requests at "/set" to receive JSON data
  server.on("/set", HTTP_POST, handleSet);

  // Start the server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();
}

// Handler for the "/status" GET endpoint
void handleStatus() {
  String message = "WiFi Status: ";
  if (WiFi.status() == WL_CONNECTED) {
    message += "Connected";
  } else {
    message += "Not connected";
  }

  // Send the response
  server.send(200, "text/plain", message);
}

// Handler for the "/led" GET endpoint (toggles the LED)
void handleLedToggle() {
  static bool ledState = LOW;  // Store the current state of the LED
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);  // Toggle the LED
  
  String response = "LED is now ";
  response += (ledState ? "ON" : "OFF");

  // Send the response
  server.send(200, "text/plain", response);
}

// Handler for the "/set" POST endpoint (receives JSON data to control the LED)
void handleSet() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    
    // Deserialize the JSON
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      server.send(400, "application/json", "{\"error\": \"Invalid JSON\"}");
      return;
    }

    // Extract the LED state from the JSON
    if (doc.containsKey("led")) {
      bool ledState = doc["led"];
      digitalWrite(ledPin, ledState ? HIGH : LOW);
      server.send(200, "application/json", "{\"status\": \"LED state updated\"}");
    } else {
      server.send(400, "application/json", "{\"error\": \"No 'led' key in the request\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\": \"No data received\"}");
  }
}
