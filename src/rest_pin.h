#ifndef REST_PIN_H
#define REST_PIN_H

#include <ESP8266WebServer.h>

void handleGetPins();
void handleGetOutputPins();
void handleGetInputPins();
void handleSetOutputPins();

#endif
