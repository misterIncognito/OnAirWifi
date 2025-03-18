#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
#include <FS.h>  // For SPIFFS

// Default AP (Access Point) credentials
const char* ap_ssid = "ESP8266-Config";
const char* ap_password = "12345678";  // Minimum 8 characters

String ssid = "default_SSID";         // Default Wi-Fi credentials
String password = "default_PASSWORD";

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
  if (ssid == "default_SSID" || password == "default_PASSWORD") {
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
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/led", HTTP_GET, handleLedToggle);
  server.on("/set", HTTP_POST, handleSet);
  server.on("/set_wifi", HTTP_POST, handleSetWiFi);

  // Start the web server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Keep blinking the LED if in AP mode
  if (WiFi.status() == WL_AP_LISTENING) {
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

// Handler for the "/status" GET endpoint
void handleStatus() {
  // Create a JSON object to structure the response
  JSONVar responseDoc;
  responseDoc["wifi_status"] = (WiFi.status() == WL_CONNECTED) ? "connected" : "not connected";
  responseDoc["ip"] = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "";

  // Serialize the JSON response
  String responseBody = JSON.stringify(responseDoc);

  // Send the JSON response
  server.send(200, "application/json", responseBody);
}

// Handler for the "/led" GET endpoint (toggles the LED)
void handleLedToggle() {
  static bool ledState = LOW;
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);

  // Create a JSON object to structure the response
  JSONVar responseDoc;
  responseDoc["led_state"] = (ledState ? "on" : "off");

  // Serialize the JSON response
  String responseBody = JSON.stringify(responseDoc);

  // Send the JSON response
  server.send(200, "application/json", responseBody);
}

// Handler for the "/set" POST endpoint (receives JSON data to control the LED)
void handleSet() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar doc;

    // Deserialize the JSON
    doc = JSON.parse(body);
    if (doc.hasOwnProperty("led")) {
      bool ledState = doc["led"];
      digitalWrite(ledPin, ledState ? HIGH : LOW);

      // Create a success response in JSON format
      JSONVar successResponse;
      successResponse["status"] = "LED state updated";

      // Serialize and send the success response
      String responseBody = JSON.stringify(successResponse);
      server.send(200, "application/json", responseBody);
    } else {
      // Create an error response in JSON format
      JSONVar errorResponse;
      errorResponse["error"] = "No 'led' key in the request";

      // Serialize and send the error response
      String responseBody = JSON.stringify(errorResponse);
      server.send(400, "application/json", responseBody);
    }
  } else {
    // Create an error response for missing data
    JSONVar errorResponse;
    errorResponse["error"] = "No data received";

    // Serialize and send the error response
    String responseBody = JSON.stringify(errorResponse);
    server.send(400, "application/json", responseBody);
  }
}

// Handler for the "/set_wifi" POST endpoint (sets new Wi-Fi credentials)
void handleSetWiFi() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JSONVar doc;

    // Deserialize the JSON
    doc = JSON.parse(body);
    if (doc.hasOwnProperty("ssid") && doc.hasOwnProperty("password")) {
      String newSSID = doc["ssid"];
      String newPassword = doc["password"];
      
      // Save new Wi-Fi credentials to SPIFFS
      saveWiFiCredentials(newSSID, newPassword);

      // Disconnect from current Wi-Fi (if connected)
      WiFi.disconnect();
      delay(1000);  // Wait a bit to ensure disconnection

      // Switch to station mode and connect to the new Wi-Fi network
      WiFi.mode(WIFI_STA);
      connectToWiFi(newSSID.c_str(), newPassword.c_str());

      // Create a success response in JSON format
      JSONVar successResponse;
      successResponse["status"] = "Wi-Fi credentials updated";
      successResponse["ssid"] = newSSID;
      successResponse["ip"] = WiFi.localIP().toString();

      // Serialize and send the success response
      String responseBody = JSON.stringify(successResponse);
      server.send(200, "application/json", responseBody);
    } else {
      // Create an error response in JSON format
      JSONVar errorResponse;
      errorResponse["error"] = "Missing 'ssid' or 'password'";

      // Serialize and send the error response
      String responseBody = JSON.stringify(errorResponse);
      server.send(400, "application/json", responseBody);
    }
  } else {
    // Create an error response for missing data
    JSONVar errorResponse;
    errorResponse["error"] = "No data received";

    // Serialize and send the error response
    String responseBody = JSON.stringify(errorResponse);
    server.send(400, "application/json", responseBody);
  }
}

// Function to load Wi-Fi credentials from SPIFFS
void loadWiFiCredentials() {
  File file = SPIFFS.open("/wifi_config.json", "r");
  if (!file) {
    Serial.println("Failed to open Wi-Fi config file.");
    return;
  }

  String fileContent = file.readString();
  JSONVar doc = JSON.parse(fileContent);
  if (doc.hasOwnProperty("ssid") && doc.hasOwnProperty("password")) {
    ssid = (const char*)doc["ssid"];
    password = (const char*)doc["password"];
  }
}

// Function to save Wi-Fi credentials to SPIFFS
void saveWiFiCredentials(const String& newSSID, const String& newPassword) {
  File file = SPIFFS.open("/wifi_config.json", "w");
  if (!file) {
    Serial.println("Failed to open Wi-Fi config file for writing.");
    return;
  }

  // Create JSON structure
  JSONVar doc;
  doc["ssid"] = newSSID;
  doc["password"] = newPassword;

  // Write JSON to file
  file.print(JSON.stringify(doc));
  file.close();
}
