#ifndef PIN_CONTROL_H
#define PIN_CONTROL_H

// Define pins
extern const int inputPin1;
extern const int inputPin2;
extern const int outputPin1;
extern const int outputPin2;

void setupPins();
void setPinMode(int pin, int mode);
void setPinState(int pin, int state);
void handleGetPins();
void handleGetOutputPins();
void handleGetInputPins();
void handleSetOutputPins();

#endif
