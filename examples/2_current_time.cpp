#include "controller/hass/integration/pin_switch.h"

#include "util/wifi_state_machine.h"
#include "util/time.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <composition.h>
#include <list>


int pin = D7;

AsyncMqttClient client;
PinSwitch pinSwitch(client, "light", pin);

WifiInfo wifiInfo = {WIFI_NETWORK, WIFI_AP_NAME, WIFI_PASSWORD};

TimeContext tc;
TimeStateMachine time_state_machine(tc);

Composition internals = { &time_state_machine };
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
