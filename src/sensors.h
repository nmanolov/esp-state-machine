#ifndef sensors_h_
#define sensors_h_

#include <AsyncMqttClient.h>
#include "observer.h"
#include "thermostat.h"
#include "integration.h"

class Sensor : public Integration {
  public:
    Sensor(AsyncMqttClient &, const char *);
    virtual void setUp();
    virtual void reconnect();
    virtual const char * getCommandTopic() const;
    virtual bool handleCommand(const char *);
  protected:
    const char * baseType() const;
    virtual void propagateState();
    void propagateConfig(bool = true);
};

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
