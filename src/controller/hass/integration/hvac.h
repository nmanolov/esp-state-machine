#ifndef hvac_h_
#define hvac_h

#include "integration.h"
#include "device/thermostat.h"

class HVAC : public Integration, public Observer<Thermostat> {
public:
    HVAC(AsyncMqttClient &, const char *, Thermostat &);
    virtual ~HVAC();
    virtual void setUp();
    virtual void reconnect();
    virtual void onChange(const Thermostat &);
    virtual bool handleCommand(const char *);
    virtual const char * willTopic() const;
    virtual const char * willPayload() const;
  protected:
    const char * baseType() const;
    virtual void propagateState();
    virtual void propagateConfig(bool = true);
  private:
    Thermostat & t;
};
 

#endif
