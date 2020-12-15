#ifndef thermostat_h_
#define thermostat_h_

#include "temperature_sensor.h"
#include <tickable.h>

class OutputPin : public Tickable, public Observable<OutputPin> {
public:
  OutputPin(const byte, const bool = false);
  void tick();
  void setUp();
  void turnOn();
  void turnOff();
  bool getState() const;

private:
  byte pin;
  bool invert;
};

class Thermostat : public Tickable, public Observable<Thermostat>, public Observer<TS_Context> {
public:
  Thermostat(TS_Context &, TemperatureSensor &, OutputPin &, float = 4);
  void tick();
  void setUp();
  void onChange(const TS_Context &);
  float getTemperature() const;
  bool getRelayValue() const;
private:
  TemperatureSensor &ts;
  TS_Context & ts_context;
  OutputPin &pin;
  float thresholdValue = 4;
};

#endif
