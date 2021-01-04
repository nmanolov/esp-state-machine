#ifndef switches_h_
#define switches_h_

#include "observer.h"
#include "integration.h"
#include "melody.h"

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

class MelodySwitch : public Switch, public Observer<Melody> {
  public:
    virtual ~MelodySwitch();
    void onChange(const Melody &);
  protected:
    Melody & m;
    MelodySwitch(AsyncMqttClient &, const char *, Melody &);
};

class PlaybackSwitch : public MelodySwitch {
  public: 
    PlaybackSwitch(AsyncMqttClient &, const char *, Melody &);
    void turnOn();
    void turnOff();
    bool getState() const;
};

class RepeatSwitch : public MelodySwitch {
  public: 
    RepeatSwitch(AsyncMqttClient &, const char *, Melody &);
    void turnOn();
    void turnOff();
    bool getState() const;
};

class PinSwitch : public Switch {
  public:
    PinSwitch(AsyncMqttClient &, const char *, const byte, const bool = false);
    void turnOn();
    void turnOff();
    bool getState() const;
    void setUp();
  private:
    byte pin;
    bool invert;
};

#endif
