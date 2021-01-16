#include "thermostat.h"
#include <Arduino.h>
#include "util/logger/logger.h"

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
