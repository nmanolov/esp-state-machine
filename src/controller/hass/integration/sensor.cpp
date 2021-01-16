#include "sensor.h"

Sensor::Sensor(AsyncMqttClient & pClient, const char * pName) : Integration(pClient, pName) { }

void Sensor::reconnect() {
  propagateConfig();
  propagateState();
}

void Sensor::setUp() { }
bool Sensor::handleCommand(const char * command) {
  return false;
}

const char * Sensor::baseType() const {
  return "sensor";
}

void Sensor::propagateConfig(bool retain) {
static const char * ConfigMessageTemplate = "{\
\"~\": \"homeassistant/sensor/%s\",\
\"stat_t\": \"~/state\",\
\"name\": \"%s\",\
\"dev\": {\
  \"ids\": \"echo1\",\
  \"mf\": \"Niki\",\
  \"name\": \"Cat feeder\",\
  \"sw\": \"0.0.23\",\
  \"mdl\": \"retro\"\
  }\
}";
  const unsigned int len = strlen(ConfigMessageTemplate) - 4 + 2 * strlen(name);
  char * message = new char[len + 1];
  sprintf(message, ConfigMessageTemplate, name, name);
  const char * configTopic = createSubTopic("config");
  publishMessage(configTopic, message, len, retain);
  delete[] configTopic;
  delete[] message;
}

void Sensor::propagateState() { }
const char * Sensor::getCommandTopic() const {
  return nullptr;
}
