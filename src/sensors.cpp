#include "sensors.h"
#include <Arduino.h>    

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

ThermostatSensor::ThermostatSensor(AsyncMqttClient & client, const char * pName, Thermostat & pMelody) : Sensor(client, pName), m(pMelody) { }

ThermostatSensor::~ThermostatSensor() {
  m.removeListener(*this);
}

void ThermostatSensor::setUp() {
  m.addListener(*this);
}

void ThermostatSensor::onChange(const Thermostat & m) {
  propagateState();
}

void ThermostatSensor::propagateConfig(bool retain) {
static const char * ConfigMessageTemperatureTemplate = "{\
\"~\": \"homeassistant/sensor/%s\",\
\"stat_t\": \"~/state\",\
\"name\": \"%s\",\
\"dev_cla\":\"temperature\",\
\"unit_of_meas\": \"°C\",\
\"val_tpl\": \"{{value_json.temperature}}\"\
}";

  unsigned int len = strlen(ConfigMessageTemperatureTemplate) - 4 + 2 * strlen(name);
  char * message = new char[len + 1];
  sprintf(message, ConfigMessageTemperatureTemplate, name, name);

  const char * configTopic = createSubTopic("config", "T");
  publishMessage(configTopic, message, len, retain);
  delete[] configTopic;
  delete[] message;

  static const char * ConfigMessageStateTemplate = "{\
\"~\": \"homeassistant/sensor/%s\",\
\"stat_t\": \"~/state\",\
\"name\": \"%s\",\
\"val_tpl\": \"{{value_json.state}}\"\
}";

  len = strlen(ConfigMessageStateTemplate) - 4 + 2 * strlen(name);
  message = new char[len + 1];
  sprintf(message, ConfigMessageStateTemplate, name, name);
  configTopic = createSubTopic("config", "S");
  publishMessage(configTopic, message, len, retain);
  delete[] configTopic;
  delete[] message;
}

void ThermostatSensor::propagateState() {
  static const char * messageTemplate = "{\
  \"temperature\": \"%f\",\
  \"state\": \"%s\"\
}";
  const unsigned int len = strlen(messageTemplate) + 10;
  char * message = new char[len+1];
  sprintf(message, messageTemplate, m.getTemperature(), m.getRelayValue()?  "on": "off");
  const char * stateTopic = createSubTopic("state");
  publishMessage(stateTopic, message);
  delete[] stateTopic;
  delete[] message;
}
