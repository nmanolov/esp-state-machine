#include "switches.h"
#include <Arduino.h>

Switch::Switch(AsyncMqttClient & pClient, const char * pName) : Integration(pClient, pName) {}

void Switch::reconnect() {
  propagateConfig();
  propagateState(getState());
  propagateAvailability(true);
}

void Switch::setUp() { }

char * Switch::createSubTopic(const char * subtopic) const {
  char * topic = new char[strlen(baseTopic()) + strlen(subtopic) + 2];
  sprintf(topic, "%s/%s", baseTopic(), subtopic);
  return topic;
}

void Switch::propagateMessage(const char * subtopic, const char * message) {
  const char * topic = createSubTopic(subtopic);
  publishMessage(topic, message);
  delete[] topic;
}

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
\"device\": {\
  \"identifiers\": \"echo1\",\
  \"manufacturer\": \"Niki\",\
  \"name\": \"Cat feeder\",\
  \"sw_version\": \"0.0.23\",\
  \"model\": \"retro\"\
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
void Switch::propagateAvailability(const bool availability) {
  propagateMessage("availability", availability? "online" : "offline");
}

bool Switch::handleCommand(const char * payload) {
  if(strcmp(payload, "ON") == 0) {
    turnOn();
  } else if (strcmp(payload, "OFF") == 0) {
    turnOff();
  }
  return true;
}

const char * Switch::getCommandTopic() const {
  char * commandTopic = new char[strlen(baseTopic()) + strlen("command") + 1];
  sprintf(commandTopic, "%s/%s", baseTopic(), "command");
  return commandTopic;
}

MelodySwitch::MelodySwitch(AsyncMqttClient & client, const char * pName, Melody & pMelody) : Switch(client, pName), m(pMelody) {
  m.addListener(*this);
}

MelodySwitch::~MelodySwitch() {
  m.removeListener(*this);
}

void MelodySwitch::onChange(const Melody & m) {
  propagateState(getState());
}

PlaybackSwitch::PlaybackSwitch(AsyncMqttClient & client, const char * name, Melody & pMelody) : MelodySwitch(client, name, pMelody) {}

void PlaybackSwitch::turnOn() {
  m.play();
}
void PlaybackSwitch::turnOff() {
  m.pause();
}
bool PlaybackSwitch::getState() const {
  return m.isPlaying();
}

RepeatSwitch::RepeatSwitch(AsyncMqttClient & client, const char * name, Melody & pMelody) : MelodySwitch(client, name, pMelody) {}
void RepeatSwitch::turnOn() {
  if(m.getRepeat()) {
    return;
  }
  m.toggleRepeat();
}

void RepeatSwitch::turnOff() { 
  if(!m.getRepeat()) {
    return;
  }
  m.toggleRepeat();
}

bool RepeatSwitch::getState() const {
  return m.getRepeat();
}

PinSwitch::PinSwitch(AsyncMqttClient & client, const char * name, const byte pinNumber, const bool pInvert):
  Switch(client, name), pin(pinNumber), invert(pInvert) {}

void PinSwitch::turnOn() {
  digitalWrite(pin, invert ^ HIGH);
  propagateState(true);
}

void PinSwitch::turnOff() {
  digitalWrite(pin, invert ^ LOW);
  propagateState(false);
}

bool PinSwitch::getState() const {
  bool value = digitalRead(pin);
  return invert ^ value;
}

void PinSwitch::setUp() {
  pinMode(pin, OUTPUT);
}
