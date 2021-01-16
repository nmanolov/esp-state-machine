#include "output_pin.h"
#include <Arduino.h>

OutputPin::OutputPin(const unsigned pinNumber, const bool pInvert)
    : pin(pinNumber), invert(pInvert) {}

void OutputPin::turnOn() { digitalWrite(pin, invert ^ HIGH); }

void OutputPin::turnOff() { digitalWrite(pin, invert ^ LOW); }

bool OutputPin::getState() const {
  bool value = digitalRead(pin);
  return invert ^ value;
}

void OutputPin::setUp() { pinMode(pin, OUTPUT); }
void OutputPin::tick() {}
