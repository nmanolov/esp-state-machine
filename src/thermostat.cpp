#include "thermostat.h"
#include <Arduino.h>

OutputPin::OutputPin(const byte pinNumber, const bool pInvert)
    : pin(pinNumber), invert(pInvert) {}

void OutputPin::turnOn() { digitalWrite(pin, invert ^ HIGH); }

void OutputPin::turnOff() { digitalWrite(pin, invert ^ LOW); }

bool OutputPin::getState() const {
  bool value = digitalRead(pin);
  return invert ^ value;
}

void OutputPin::setUp() { pinMode(pin, OUTPUT); }
void OutputPin::tick() {}

Thermostat::Thermostat(TS_Context &pContext, TemperatureSensor &pTs,
                       OutputPin &pPin, float pThreshold)
    : ts(pTs), ts_context(pContext), pin(pPin), thresholdValue(pThreshold) {
  pContext.addListener(*this);
};
void Thermostat::tick() {
  ts.tick();
  pin.tick();
};
void Thermostat::setUp() {
  pin.setUp();
  ts.setUp();
};
void Thermostat::onChange(const TS_Context &context) {
  if (context.getTemperature() <= thresholdValue) {
    pin.turnOn();
  } else {
    pin.turnOff();
  }
  notifyObservers(*this);
};

float Thermostat::getTemperature() const {
  return ts_context.getTemperature();
};

bool Thermostat::getRelayValue() const { return pin.getState(); };
