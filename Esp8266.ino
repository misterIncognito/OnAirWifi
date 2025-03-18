#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Wi-Fi credentials (Initial values)
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
  connectToWiFi(ssid, password);

  // Handle GET requests at the root path ("/status")
  server.on("/status", HTTP_GET, handleStatus);
  
  // Handle GET requests for "/led" to toggle the LED
  server.on("/led", HTTP_GET, handleLedToggle);
  
  // Handle POST requests at "/set" to receive JSON data
  server.on("/set", HTTP_POST, handleSet);
  
  // Handle POST requests to change Wi-Fi credentials
  server.on("/set_wifi", HTTP_POST, handleSetWiFi);

  // Start the server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();
}

// Function to connect to Wi-Fi
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

// Handler for the "/set_wifi" POST endpoint (sets new Wi-Fi credentials)
void handleSetWiFi() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    
    // Deserialize the JSON
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      server.send(400, "application/json", "{\"error\": \"Invalid JSON\"}");
      return;
    }

    // Extract the SSID and password from the JSON
    if (doc.containsKey("ssid") && doc.containsKey("password")) {
      String newSSID = doc["ssid"];
      String newPassword = doc["password"];
      
      // Disconnect from the current Wi-Fi network
      WiFi.disconnect();
      delay(1000);  // Wait a bit to ensure disconnection

      // Attempt to connect to the new Wi-Fi network
      connectToWiFi(newSSID.c_str(), newPassword.c_str());

      // Send a success response
      String response = "{\"status\": \"Wi-Fi credentials updated\", ";
      response += "\"ssid\": \"" + newSSID + "\", ";
      response += "\"ip\": \"" + WiFi.localIP().toString() + "\"}";
      server.send(200, "application/json", response);
    } else {
      server.send(400, "application/json", "{\"error\": \"Missing 'ssid' or 'password'\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\": \"No data received\"}");
  }
}
