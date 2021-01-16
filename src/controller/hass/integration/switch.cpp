#include "switch.h"
#include <Arduino.h>

Switch::Switch(AsyncMqttClient & pClient, const char * pName) : Integration(pClient, pName) {}

void Switch::reconnect() {
  propagateConfig();
  propagateState(getState());
  propagateAvailability(true);
}

void Switch::setUp() { }

const char * Switch::baseType() const {
  return "switch";
}

void Switch::propagateConfig(bool retain) {
static const char * ConfigMessageTemplate = "{\
\"~\": \"homeassistant/switch/%s\",\
\"avty_t\": \"~/availability\",\
\"cmd_t\": \"~/command\",\
\"stat_t\": \"~/state\",\
\"name\": \"%s\",\
\"retain\": true,\
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

void Switch::propagateState(const bool state) {
  propagateMessage("state", state? "ON" : "OFF");
}

bool Switch::handleCommand(const char * payload) {
  if(strcmp(payload, "ON") == 0) {
    turnOn();
  } else if (strcmp(payload, "OFF") == 0) {
    turnOff();
  }
  return true;
}
