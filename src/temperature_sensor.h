#ifndef temperature_sensor_h
#define temperature_sensor_h
#include <Arduino.h>
#include "observer.h"
#include "n_state.h"
#include <OneWire.h>

class TS_Context : public Observable<TS_Context> {
public:
  TS_Context(int);
  void setTemperature(float);
  float getTemperature() const;
  const int pin;
  OneWire wire;
  byte addr[8];
  byte type_s;
private:
  float celsius;
};

enum TS_Sygnals {
  TS_Initialize,
  TS_Timeout,
  TS_Tick,
};

typedef NState<TS_Sygnals, TS_Context> TS_State;

TS_State * searchForAddresses(TS_State *, StateMachineHandle<TS_Context> &, const TS_Sygnals &);
TS_State * reseting(TS_State *, StateMachineHandle<TS_Context> &, const TS_Sygnals &);
TS_State * writing(TS_State *, StateMachineHandle<TS_Context> &, const TS_Sygnals &);
TS_State * processing(TS_State *, StateMachineHandle<TS_Context> &, const TS_Sygnals &);

class TemperatureSensor : public NStateMachine<TS_Sygnals, TS_Context> {
  public:
    TemperatureSensor(TS_Context &);
};

#endif
