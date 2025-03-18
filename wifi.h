#ifndef WIFI_H
#define WIFI_H

extern String ssid;
extern String password;

void connectToWiFi(const char* ssid, const char* password);
void loadWiFiCredentials();
void saveWiFiCredentials(String ssid, String password);

#endif
