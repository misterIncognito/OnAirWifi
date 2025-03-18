#ifndef REST_WIFI_H
#define REST_WIFI_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;
extern String ssid;
extern String password;

void handleGetWiFi();
void handleSetWiFi();
void handleResetWiFi();


#endif
