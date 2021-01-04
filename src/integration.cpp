#include "integration.h"

Integration::Integration(AsyncMqttClient & pClient, const char * pName, const bool pHasWill) : client(pClient), bT(nullptr), mHasWill(pHasWill) {
  int len = strlen(pName);
  name = new char[len + 1];
  strcpy(name, pName);
}

Integration::~Integration() {
  delete name;
  if (bT != nullptr) {
    delete[] bT;
  }
};

void Integration::publishMessage(const char * topic, const char * message, const bool retain) {
 client.publish(topic, 0, retain, message, strlen(message));
}

void Integration::publishMessage(const char * topic, const char * message, unsigned int len, const bool retain) {
  client.publish(topic, 0, retain, message, len);
}

const char * Integration::baseTopic() const {
  if(bT == nullptr) {
    unsigned typeLength = strlen(baseType());
    unsigned int len = strlen("homeassistant/") + typeLength + strlen(name) + 1;
    bT = new char[len + 1];
    sprintf(bT, "homeassistant/%s/%s", baseType(), name);
  }
  return bT;
}

const char * Integration::createSubTopic(const char * subtopic, const char * nameSuffix) const {
  char * topic = new char[strlen(baseTopic()) + strlen(subtopic) + 2];
  sprintf(topic, "%s%s/%s", baseTopic(), nameSuffix, subtopic);
  return topic;
}
void Integration::propagateMessage(const char * subtopic, const char * message, const bool retain) {
  const char * topic = createSubTopic(subtopic);
  publishMessage(topic, message, retain);
  delete[] topic;
}

void Integration::propagateAvailability(const bool availability) {
  propagateMessage("availability", availability? "online" : "offline", true);
}

bool Integration::hasWill() const {
  return mHasWill;
}

const char * Integration::willTopic() const {
  return "";
}
const char * Integration::willPayload() const {
  return "";
}

const char * Integration::getCommandTopic() const {
  char * commandTopic = new char[strlen(baseTopic()) + strlen("command") + 1];
  sprintf(commandTopic, "%s/%s", baseTopic(), "command");
  return commandTopic;
}
