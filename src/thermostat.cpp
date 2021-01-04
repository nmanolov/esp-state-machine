#include "thermostat.h"
#include <Arduino.h>
#include <logger.h>

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
                       OutputPin &pPin, float pTarget)
    : ts(pTs), ts_context(pContext), pin(pPin), targetValue(pTarget) {
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
  float currentTemperature = context.getTemperature();
  if (currentTemperature <= targetValue - 1) {
    pin.turnOn();
  }
  if (currentTemperature >= targetValue + 1 ){
    pin.turnOff();
  }
  notifyObservers(*this);
};

float Thermostat::getTemperature() const {
  return ts_context.getTemperature();
};

float Thermostat::getTarget() const { return targetValue; }

void Thermostat::setTarget(const float &pTarget) {
  targetValue = pTarget;

  onChange(ts_context);
}

bool Thermostat::getRelayValue() const { return pin.getState(); };
