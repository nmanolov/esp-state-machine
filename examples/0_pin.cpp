#include "controller/hass/integration/pin_switch.h"
#include "util/wifi_state_machine.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <composition.h>
#include <list>

int pin = D7;

AsyncMqttClient client;
PinSwitch pinSwitch(client, "light", pin);

std::list<Integration *> integrations = {&pinSwitch};
WifiInfo wifiInfo = {WIFI_NETWORK, WIFI_AP_NAME, WIFI_PASSWORD};

MqttClientInfo mqttClientInfo = {
    IPAddress(MQTT_BROCKER_ADDRESS),
    MQTT_BROCKER_PORT,
    MQTT_CLIENT_ID,
    MQTT_CLIENT_NAME,
    MQTT_CLIENT_PASSWORD,
};
ClientContext cs = {
    client,
    integrations,
    mqttClientInfo,
};
ClientSM clientStateMachine(cs);
Composition internals = {&clientStateMachine};

WifiContext wifiContext = {
    WiFi,
    wifiInfo,
    internals,
};
WiFiSM wifiSm(wifiContext);

void setup() {
  Serial.begin(9600);
  wifiSm.setUp();
}
void loop() {
  wifiSm.tick();
}
