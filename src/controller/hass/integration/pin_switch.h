#ifndef pin_switch_h
#define pin_switch_h

#include "switch.h"
#include "output_pin.h"

class PinSwitch : public Switch {
  public:
    PinSwitch(AsyncMqttClient &, const char *, const byte, const bool = false);
    void turnOn();
    void turnOff();
    bool getState() const;
    void setUp();
  private:
    OutputPin pin;
};

#endif
