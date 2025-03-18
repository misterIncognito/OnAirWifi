# OnAirWifi
ESP Module On-Air LED Control Over Wi-Fi

![alt text](image.png)

This repository contains code for controlling an On-Air LED indicator using an ESP module (ESP8266 or ESP32) over Wi-Fi. The LED can be turned on or off remotely via a web interface or REST API, making it ideal for applications such as broadcast studios, streaming setups, or any other scenario where remote LED control is needed.

Features:
* Wi-Fi Connectivity: Connects to a local Wi-Fi network to communicate with the control server.
* Web Interface: Easy-to-use web-based UI for turning the LED on or off.
* REST API: Control the LED programmatically with simple HTTP requests.
* Lightweight: Optimized for use with ESP8266/ESP32 boards, ensuring minimal resource usage.

Requirements:
* ESP8266 or ESP32 module
* Arduino IDE or PlatformIO for compiling and uploading the code
* Wi-Fi network for communication

Installation:
* Clone or download the repository.
* Open the code in your preferred IDE (Arduino IDE or PlatformIO).
* Modify the Wi-Fi credentials in the code.
* Upload the code to the ESP module.
* Access the web interface via the ESP module’s IP address to control the LED.
