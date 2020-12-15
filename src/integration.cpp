#include "integration.h"

Integration::Integration(AsyncMqttClient & pClient, const char * pName) : client(pClient), bT(nullptr) {
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
