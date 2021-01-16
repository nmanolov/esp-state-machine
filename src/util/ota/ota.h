#ifndef ota_h_
#define ota_h_

#include <tickable.h>
#include <ArduinoOTA.h>

struct OTA_Options {
  const char * auth;
  const unsigned int port;
};

class OTA : public Tickable {
public:
  OTA(ArduinoOTAClass &, const OTA_Options &);
  void setUp();
  void tick();

private:
  ArduinoOTAClass & ArduinoOTA;
  const OTA_Options & options;
};

#endif
