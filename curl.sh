# Replace <ESP_IP> with actual IP, default value is 192.168.4.1

# 1. Get the status of the ESP8266 device (Check Wi-Fi connection status and IP address)
curl -X GET http://<ESP_IP>/status

# Example Response:
# {
#   "wifi_status": "connected",
#   "ip": "192.168.1.10"
# }

# 2. Get the current LED state (on/off)
curl -X GET http://<ESP_IP>/led

# Example Response:
# {
#   "led_state": "on"
# }

# 3. Toggle LED state (POST request with JSON body)
curl -X POST http://<ESP_IP>/led \
     -H "Content-Type: application/json" \
     -d '{"led": true}'

# Example Response:
# {
#   "status": "LED state updated"
# }

# 4. Set new Wi-Fi credentials (POST request with JSON body)
curl -X POST http://<ESP_IP>/wifi \
     -H "Content-Type: application/json" \
     -d '{"ssid": "NewSSID", "password": "NewPassword"}'

# Example Response:
# {
#   "status": "Wi-Fi credentials updated",
#   "ssid": "NewSSID",
#   "ip": "192.168.1.10"
# }

# 5. GET health/status of the device (check if connected to Wi-Fi)
curl -X GET http://<ESP_IP>/health
