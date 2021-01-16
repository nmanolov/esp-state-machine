#ifndef thermostat_h_
#define thermostat_h_

#include "./temperature_sensor.h"
#include <tickable.h>
#include "output_pin.h"

class Thermostat : public Tickable, public Observable<Thermostat>, public Observer<TS_Context> {
public:
  Thermostat(TS_Context &, TemperatureSensor &, OutputPin &, float = 4);
  void tick();
  void setUp();
  void onChange(const TS_Context &);
  float getTemperature() const;
  float getTarget() const;
  void setTarget(const float &);
  bool getRelayValue() const;
private: 
  void onChange();
  TemperatureSensor &ts;
  TS_Context & ts_context;
  OutputPin &pin;
  float targetValue = 4;
};

#endif
