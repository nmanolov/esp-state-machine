#include "hvac.h"
#include <Arduino.h>
#include <logger.h> 

HVAC::HVAC(AsyncMqttClient & pClient, const char * pName, Thermostat & pThermostat):
  Integration(pClient, pName, true), t(pThermostat) {}

HVAC::~HVAC() {
  t.removeListener(*this);
}

void HVAC::setUp() {
  t.addListener(*this);
}

void HVAC::reconnect() {
  propagateConfig();
  propagateAvailability(true);
  propagateState();
}

const char * HVAC::baseType() const {
  return "climate";
}

bool HVAC::handleCommand(const char * command) {
  float nextTarget = std::atof(command);
  Logger.printf("Command: %s - %f\r\n", command, nextTarget);

  if(isnan(nextTarget) || isinf(nextTarget)) {
    return false;
  }

  t.setTarget(nextTarget);
  return true;
} 

void HVAC::onChange(const Thermostat & m) {
  propagateState();
}

void HVAC::propagateState() {
  static const char * messageTemplate = "{\
  \"current\": %f,\
  \"mode\": \"%s\",\
  \"target\": %f\
}";
  const unsigned int len = strlen(messageTemplate) + 20;
  char * message = new char[len+1];
  sprintf(
    message,
    messageTemplate,
    t.getTemperature(),
    t.getRelayValue()?  "heat": "off",
    t.getTarget());

const char * stateTopic = createSubTopic("state");
  publishMessage(stateTopic, message);
  delete[] stateTopic;
  delete[] message;
}

void HVAC::propagateConfig(bool retain) {
static const char * Template = "{\
  \"name\": \"%s\",\
  \"modes\": [ \"off\", \"heat\" ],\
  \"~\": \"homeassistant/climate/%s\",\
  \"curr_temp_t\": \"~/state\",\
  \"mode_stat_t\": \"~/state\",\
  \"temp_stat_t\": \"~/state\",\
  \"temp_cmd_t\": \"~/command\",\
  \"curr_temp_tpl\": \"{{ value_json.current }}\",\
  \"mode_stat_tpl\": \"{{ value_json.mode }}\",\
  \"temp_stat_tpl\": \"{{ value_json.target }}\",\
  \"avty_t\": \"~/availability\",\
  \"temp_step\": 0.1,\
  \"precision\": 0.1,\
  \"min_temp\": 1,\
  \"max_temp\": 10,\
  \"init\": 4,\
  \"retain\": true,\
  \"uniq_id\": \"%s\", \
  \"dev\": {\
    \"ids\": \"echo1\",\
    \"mf\": \"Niki\",\
    \"name\": \"%s\",\
    \"sw\": \"0.0.23\",\
    \"mdl\": \"retro\"\
  }\
}";

  const unsigned int len = strlen(Template) + 4 * (strlen(name) - 2);
  char * message = new char[len + 1];
  sprintf(message, Template, name, name, name, name);
  const char * configTopic = createSubTopic("config");
  publishMessage(configTopic, message, len, retain);
  delete[] configTopic;
  delete[] message;
}
 
const char * HVAC::willTopic() const {
  return createSubTopic("availability");
}

const char * HVAC::willPayload() const {
  return "offline";
}
