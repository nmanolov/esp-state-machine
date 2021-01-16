
#include "device/thermostat.h"

#include "controller/hass/mqtt_client_state_machine.h"
#include "controller/hass/integration/sensor.h"
#include "controller/hass/integration/switch.h"
#include "controller/hass/integration/thermostat_sensor.h"
#include "controller/hass/integration/hvac.h"

#include "util/wifi_state_machine.h"
#include "util/ota/ota.h"
#include "util/logger/telnet_logger.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <composition.h>
#include <list>


TS_Context ts_context(D6);
TemperatureSensor sensor(ts_context);
OutputPin relayPin(D7);

Thermostat thermostat(ts_context, sensor, relayPin, THERMOSTAT_TARGET);

AsyncMqttClient client;

HVAC hvac(client, THERMOSTAT_NAME, thermostat);
std::list<Integration *> integrations = { &hvac };

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

Composition internals = { &clientStateMachine };
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
