#include "button.h"
#include <Arduino.h>

Button::Button(unsigned pPin, PressHandler pHandler, unsigned pDebounce):
  pin(pPin),
  handler(pHandler),
  debounceTime(pDebounce) {}

void Button::tick() {
  int currentReading = digitalRead(pin);
  unsigned long currentTime = millis();

  if (currentReading == HIGH
    && lastReading == LOW
    && currentTime - time > debounceTime) {
    handler();
    time = currentTime;
  }

  lastReading = currentReading;
}

void Button::setUp() {
  pinMode(pin, INPUT);
}
