#include "melody.h"
#include "pitches.h"

#include "controller/serial_command.h"
#include "controller/hass/integration/melody_switches.h"

#include "util/wifi_state_machine.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <composition.h>
#include <list>


int melodyTones[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4,
  NOTE_C4,
  0, 0, 0, 0,
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 4, 4,
  4, 4, 2,
  4, 4, 4, 4,
  2, 2,
  1, 1, 1, 1
};
int Sound = D7;
Melody melody(Sound, melodyTones, noteDurations, 17, 1000, true);
Command command(melody);

AsyncMqttClient client;
PlaybackSwitch ms(client, "playback", melody);
RepeatSwitch rs(client, "repeat", melody);

std::list<Integration *> integrations = {&ms, &rs};
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
  command.tick();
}
void loop() {
  wifiSm.tick();
  command.tick();
}
