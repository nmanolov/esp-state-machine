
#include "button.h"
#include "command.h"
#include "finite_state_machine.h"
#include "mqtt_client_state_machine.h"
#include "sensors.h"
#include "switches.h"
#include "temperature_sensor.h"
#include "thermostat.h"
#include "time.h"
#include "wifi_state_machine.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <composition.h>
#include <list>
#include <logger.h>
#include <n_state.h>
#include <ota.h>
#include <telnet_logger.h>

#include "pitches.h"

// int melodyTones[] = {
//   NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
//   NOTE_G4, NOTE_G4, NOTE_G4,
//   NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
//   NOTE_C4,
//   NOTE_C4,
//   0, 0, 0, 0,
// };

// // note durations: 4 = quarter note, 8 = eighth note, etc.:
// int noteDurations[] = {
//   4, 4, 4, 4,
//   4, 4, 2,
//   4, 4, 4, 4,
//   2, 2,
//   1, 1, 1, 1
// };
// int Sound = D7;
// Melody melody(Sound, melodyTones, noteDurations, 17, 1000, true);
// Command command(melody);

TS_Context ts_context(D6);
TemperatureSensor sensor(ts_context);
OutputPin relayPin(D7);

Thermostat thermostat(ts_context, sensor, relayPin, THERMOSTAT_THRESHOLD);

AsyncMqttClient client;
// PlaybackSwitch ms(client, "playback", melody);
// RepeatSwitch rs(client, "repeat", melody);

ThermostatSensor ts(client, THERMOSTAT_NAME, thermostat);
// PinSwitch ps(client, "led", LED_BUILTIN, true);
std::list<Integration *> integrations = {&ts /*&ps, &ts, &ms, &rs*/};
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
// // TimeContext tc;
// // TimeStateMachine tsm(tc);
Composition internals = {&clientStateMachine /*, &tsm*/};
WifiContext wifiContext = {
    WiFi,
    wifiInfo,
    internals,
};
WiFiSM wifiSm(wifiContext);

OTA_Options otaOptions = {
    OTA_PASSWORD,
    OTA_PORT,
};
OTA ota(ArduinoOTA, otaOptions);

WiFiServer telnetServer(TELNET_PORT);
TelnetLogger telnetLogger(telnetServer);
SerialLogger serialLogger;

Composition composition = {
    &wifiSm,
    &thermostat,
    &serialLogger,
    &ota,
    &telnetLogger,
};


void setup() {
  Logger.registerLogger(&telnetLogger);
  Logger.registerLogger(&serialLogger);
  composition.setUp();
}
void loop() { composition.tick(); }
