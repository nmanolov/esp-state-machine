#ifndef mqtt_integrations_
#define mqtt_integrations_

#include <AsyncMqttClient.h>

class Integration {
public:
  Integration(AsyncMqttClient &, const char *, const bool = false);
  virtual ~Integration();
  virtual void setUp() = 0;
  virtual void reconnect() = 0;
  virtual const char *getCommandTopic() const;
  virtual bool handleCommand(const char *) = 0;
  virtual void propagateMessage(const char *, const char *, const bool = false);
  virtual void propagateConfig(bool = true) = 0;
  virtual bool hasWill() const final;
  virtual const char * willTopic() const;
  virtual const char * willPayload() const;
protected:
  virtual void propagateAvailability(const bool);
  virtual const char * createSubTopic(const char *, const char * = "") const;
  virtual const char * baseType() const = 0;
  virtual const char * baseTopic() const;
  virtual void publishMessage(const char *, const char *, unsigned int, const bool = false);
  virtual void publishMessage(const char *, const char *, const bool = false);

  char *name;
private:
  AsyncMqttClient &client;
  const bool mHasWill;
  mutable char *bT;
};

#endif
