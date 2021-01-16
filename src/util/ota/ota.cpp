#include "./ota.h"
#include "util/logger/logger.h"

OTA::OTA(ArduinoOTAClass &pArduinoOta, const OTA_Options &pOptions)
    : ArduinoOTA(pArduinoOta), options(pOptions) {}

void OTA::setUp() {
  ArduinoOTA.setPassword(options.auth);
  ArduinoOTA.setPort(options.port);
  ArduinoOTA.onStart([]() { Logger.println("OTA Start"); });
  ArduinoOTA.onEnd([]() {
    Logger.println("OTA End");
    Logger.println("Rebooting...");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Logger.printf(">>>Progress: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Logger.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Logger.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Logger.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Logger.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Logger.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Logger.println("End Failed");
  });
  ArduinoOTA.begin();
}

void OTA::tick() { ArduinoOTA.handle(); }
