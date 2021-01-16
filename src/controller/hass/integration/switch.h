#ifndef switch_h_
#define switch_h_

#include "integration.h"

class Switch : public Integration {
  public:
    Switch(AsyncMqttClient &, const char *);
    virtual void setUp();
    virtual void reconnect();
    virtual bool handleCommand(const char *);
  protected:
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual bool getState() const = 0;
    const char * baseType() const;
    virtual void propagateConfig(bool = true);
    void propagateState(const bool);
    void propagateMessage(const char *, const char *);
};

#endif
