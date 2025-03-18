#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include <FS.h>  // For SPIFFS

// Default AP (Access Point) credentials
const char* ap_ssid = "On-AirLEDWiFi";
const char* ap_password = "12345678";  // Minimum 8 characters

String ssid = "x";         // Default Wi-Fi credentials changed to "x"
String password = "x";     // Default Wi-Fi credentials changed to "x"

const int ledPin = LED_BUILTIN;  // Built-in LED pin (pin 2 for many boards)

// Define input pins
const int inputPin1 = D1;
const int inputPin2 = D2;

// Define output pins
const int outputPin1 = D3;
const int outputPin2 = D4;

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

  // Set input pins as inputs
  pinMode(inputPin1, INPUT);
  pinMode(inputPin2, INPUT);

  // Set output pins as outputs
  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);

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

  // Set up the server endpoints
  server.on("/wifi", HTTP_GET, handleGetWiFi);        // Get current Wi-Fi credentials
  server.on("/wifi", HTTP_POST, handleSetWiFi);       // Set new Wi-Fi credentials
  server.on("/wifi/reset", HTTP_GET, handleResetWiFi); // Reset Wi-Fi credentials to default
  server.on("/led", HTTP_GET, handleGetLed);          // Get current LED state
  server.on("/led", HTTP_POST, handleSetLed);         // Set LED state
  server.on("/health", HTTP_GET, handleHealth);       // Check health/status of the device

  // New endpoints to control pins
  server.on("/pin/input", HTTP_GET, handleGetInputPins);  // Get input pin states
  server.on("/pin/input", HTTP_POST, handleSetInputPins); // Set input pin states
  server.on("/pin/out", HTTP_GET, handleGetOutputPins);  // Get output pin states
  server.on("/pin/out", HTTP_POST, handleSetOutputPins); // Set output pin states

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

// Blink the LED
void blinkLED() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Save the last time LED blinked
    previousMillis = currentMillis;

    // If the LED is on, turn it off, and vice versa
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

// Handler for the "/pin/input" GET endpoint (get input pin states)
void handleGetInputPins() {
  JSONVar responseDoc;
  
  responseDoc["inputPin1"] = digitalRead(inputPin1);
  responseDoc["inputPin2"] = digitalRead(inputPin2);

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

// Handler for the "/pin/input" POST endpoint (set input pin states)
void handleSetInputPins() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);
    if (JSON.typeof(json) == "undefined") {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    int pin1State = json["inputPin1"];
    int pin2State = json["inputPin2"];

    // Set the input pins to the requested states (this could be used for debouncing or other logic)
    // Note: INPUT pins can't be directly set in a typical way, so you'd probably just check the values.

    JSONVar responseDoc;
    responseDoc["status"] = "Input pins states received";

    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
  }
}

// Handler for the "/pin/out" GET endpoint (get output pin states)
void handleGetOutputPins() {
  JSONVar responseDoc;
  
  responseDoc["outputPin1"] = digitalRead(outputPin1);
  responseDoc["outputPin2"] = digitalRead(outputPin2);

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

// Handler for the "/pin/out" POST endpoint (set output pin states)
void handleSetOutputPins() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar json = JSON.parse(body);
    if (JSON.typeof(json) == "undefined") {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    bool pin1State = json["outputPin1"];
    bool pin2State = json["outputPin2"];

    // Set the output pins to the requested states
    digitalWrite(outputPin1, pin1State ? HIGH : LOW);
    digitalWrite(outputPin2, pin2State ? HIGH : LOW);

    JSONVar responseDoc;
    responseDoc["status"] = "Output pins states updated";

    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
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

// Handler for the "/wifi" POST endpoint (set Wi-Fi credentials)
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

    // Save new Wi-Fi credentials to the file system
    saveWiFiCredentials(new_ssid, new_password);

    // Restart the ESP8266 with new credentials
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(new_ssid.c_str(), new_password.c_str());

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    // Send a response with new IP address
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

// Handler for the "/wifi/reset" GET endpoint (reset Wi-Fi credentials)
void handleResetWiFi() {
  // Reset Wi-Fi credentials to default values
  ssid = "x";
  password = "x";

  // Save default credentials to the file system
  saveWiFiCredentials(ssid, password);

  // Reboot the ESP8266
  ESP.restart();

  // Send response before rebooting
  JSONVar responseDoc;
  responseDoc["status"] = "Wi-Fi credentials reset to default";
  responseDoc["ssid"] = ssid;
  responseDoc["password"] = password;

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

// Handler for the "/led" GET endpoint (get current LED state)
void handleGetLed() {
  JSONVar responseDoc;
  responseDoc["led_state"] = (ledState ? "on" : "off");

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

// Handler for the "/led" POST endpoint (set LED state)
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

    // Create a JSON object to structure the response
    JSONVar responseDoc;
    responseDoc["status"] = "LED state updated";

    String responseBody = JSON.stringify(responseDoc);
    server.send(200, "application/json", responseBody);
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
  }
}

// Handler for the "/health" GET endpoint (check device health/status)
void handleHealth() {
  JSONVar responseDoc;
  responseDoc["status"] = (WiFi.status() == WL_CONNECTED) ? "connected" : "disconnected";
  responseDoc["ip"] = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "";

  String responseBody = JSON.stringify(responseDoc);
  server.send(200, "application/json", responseBody);
}

// Load Wi-Fi credentials from the SPIFFS (persistent storage)
void loadWiFiCredentials() {
  File file = SPIFFS.open("/wifi_credentials.json", "r");
  if (file) {
    String fileContent = file.readString();
    JSONVar json = JSON.parse(fileContent);
    if (JSON.typeof(json) != "undefined") {
      // Use .as<String>() method to convert to String
      ssid = String(json["ssid"]);
      password = String(json["password"]);
    }
    file.close();
  }
}

// Function to save Wi-Fi credentials to the SPIFFS (persistent storage)
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
