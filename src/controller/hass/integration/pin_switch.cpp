#include "pin_switch.h"


PinSwitch::PinSwitch(AsyncMqttClient & client, const char * name, const byte pinNumber, const bool pInvert):
  Switch(client, name), pin(pinNumber, pInvert) {}

void PinSwitch::turnOn() {
  pin.turnOn();
  propagateState(true);
}

void PinSwitch::turnOff() {
  pin.turnOff();
  propagateState(false);
}

bool PinSwitch::getState() const {
  return pin.getState();
}

void PinSwitch::setUp() {
  pin.setUp();
}
