#ifndef mqtt_integrations_
#define mqtt_integrations_

#include <AsyncMqttClient.h>

class Integration {
public:
  Integration(AsyncMqttClient &, const char *);
  virtual ~Integration();
  virtual void setUp() = 0;
  virtual void reconnect() = 0;
  virtual const char *getCommandTopic() const = 0;
  virtual bool handleCommand(const char *) = 0;
  virtual void propagateMessage(const char *, const char *) = 0;
  virtual void propagateConfig(bool = true) = 0;
protected:
  virtual const char * baseType() const = 0;
  virtual const char * baseTopic() const;
  virtual void publishMessage(const char *, const char *, unsigned int, const bool = false);
  virtual void publishMessage(const char *, const char *, const bool = false);

  char *name;
private:
  AsyncMqttClient &client;
  mutable char *bT;
};

#endif
