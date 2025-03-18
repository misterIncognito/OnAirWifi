#ifndef REST_LED_H
#define REST_LED_H

extern const int ledPin;
extern bool ledState;
extern unsigned long previousMillis;
extern const long interval;

void handleGetLed();
void handleSetLed();

#endif
