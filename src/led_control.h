#ifndef LED_CONTROL_H
#define LED_CONTROL_H

extern const int ledPin;
extern bool ledState;
extern unsigned long previousMillis;
extern const long interval;

void blinkLED();

#endif
