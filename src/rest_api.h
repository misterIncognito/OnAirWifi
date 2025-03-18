#ifndef REST_API_H
#define REST_API_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

void setupRestApi();
void handleGetHealth();
void handleGetWiFi();
void handleSetWiFi();
void handleResetWiFi();
void handleGetLed();
void handleSetLed();

void handleGetPins();
void handleGetOutputPins();
void handleGetInputPins();
void handleSetOutputPins();
#endif
