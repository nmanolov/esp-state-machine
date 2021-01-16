#ifndef telnet_logger_
#define telnet_logger_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <tickable.h>

#include "logger.h"

class TelnetLogger : public LoggerClass {
public:
  TelnetLogger(WiFiServer &);
  virtual ~TelnetLogger();
  void setUp();
  void tick();

  bool checkClient();

  bool printf(const char *format, ...) __attribute__((format(printf, 2, 3)));
  bool print(const char *);
  bool print(unsigned int, int = DEC);
  bool println(const char *);
  bool println(char);
  bool println(unsigned int, int = DEC);
  bool println(const Printable &);
  bool println(void);

private:
  WiFiServer &server;
  WiFiClient client;
};

#endif
