#ifndef sensor_h_
#define sensor_h_

#include "observer.h"
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

#endif
