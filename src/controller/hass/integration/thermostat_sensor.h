#ifndef sensors_h_
#define sensors_h_

#include "device/thermostat.h"
#include "sensor.h"

class ThermostatSensor : public Sensor, public Observer<Thermostat> {
  public:
    ThermostatSensor(AsyncMqttClient &, const char *, Thermostat &);
    virtual ~ThermostatSensor();
    void onChange(const Thermostat &);
    void setUp();
  protected:
    void propagateState();
    void propagateConfig(bool = true);
    Thermostat & m;
};

#endif
