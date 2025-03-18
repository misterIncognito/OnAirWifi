#include <ESP8266WiFi.h>

const char* ssid = "your_SSID";          // Replace with your network name
const char* password = "your_PASSWORD";  // Replace with your network password

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  
  // Wait for the Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Once connected, print the IP address
  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Main loop code
}
