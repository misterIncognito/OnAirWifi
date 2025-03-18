#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>  // For SPIFFS

// Default AP (Access Point) credentials
const char* ap_ssid = "ESP8266-Config";
const char* ap_password = "12345678";  // Minimum 8 characters

String ssid = "default_SSID";         // Default Wi-Fi credentials
String password = "default_PASSWORD";

const int ledPin = LED_BUILTIN;  // Built-in LED pin (pin 2 for many boards)

ESP8266WebServer server(80);  // Create an instance of the server, listening on port 80

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

  // Start in AP mode if no Wi-Fi credentials are saved or invalid
  if (ssid == "default_SSID" || password == "default_PASSWORD") {
    // Start ESP as an Access Point
    WiFi.softAP(ap_ssid, ap_password);  // Start Access Point with specified SSID and password
    Serial.println("AP Mode: Waiting for configuration...");

    // Print the IP address of the Access Point
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    // Attempt to connect to the Wi-Fi network with stored credentials
    connectToWiFi(ssid.c_str(), password.c_str());
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
  static bool ledState = LOW;
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);

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
      
      // Save new Wi-Fi credentials to SPIFFS
      saveWiFiCredentials(newSSID, newPassword);

      // Disconnect from current Wi-Fi (if connected)
      WiFi.disconnect();
      delay(1000);  // Wait a bit to ensure disconnection

      // Switch to station mode and connect to the new Wi-Fi network
      WiFi.mode(WIFI_STA);
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

// Function to load Wi-Fi credentials from SPIFFS
void loadWiFiCredentials() {
  File file = SPIFFS.open("/wifi_config.json", "r");
  if (!file) {
    Serial.println("Failed to open Wi-Fi config file. Using default credentials.");
    return;
  }

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to read Wi-Fi config file. Using default credentials.");
    return;
  }

  if (doc.containsKey("ssid") && doc.containsKey("password")) {
    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();
  }
}

// Function to save Wi-Fi credentials to SPIFFS
void saveWiFiCredentials(const String& newSSID, const String& newPassword) {
  File file = SPIFFS.open("/wifi_config.json", "w");
  if (!file) {
    Serial.println("Failed to open Wi-Fi config file for writing.");
    return;
  }

  StaticJsonDocument<200> doc;
  doc["ssid"] = newSSID;
  doc["password"] = newPassword;

  // Serialize the JSON to the file
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to Wi-Fi config file.");
  }

  file.close();
}
