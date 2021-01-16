#include "./telnet_logger.h"

TelnetLogger::TelnetLogger(WiFiServer &pServer) : server(pServer) {}
TelnetLogger::~TelnetLogger() {}
void TelnetLogger::setUp() { server.begin(); }

void TelnetLogger::tick() {
  if (server.hasClient()) {
    // client is connected
    if (!client || !client.connected()) {
      if (client)
        client.stop();             // client disconnected
      client = server.available(); // ready for new client
    } else {
      server.available().stop(); // have client, block new conections
    }
  }
}

bool TelnetLogger::checkClient() {
  return client && client.connected() && client.available();
}

bool TelnetLogger::println(const char *word) {
  if (!checkClient()) {
    return false;
  }
  return client.println(word);
}
bool TelnetLogger::print(const char *word) {
  if (!checkClient()) {
    return false;
  }

  return client.print(word);
}
bool TelnetLogger::println(char c) {
  if (!checkClient()) {
    return false;
  }

  return client.println(c);
}
bool TelnetLogger::print(unsigned int number, int base) {
  if (!checkClient()) {
    return false;
  }

  return client.print(number, base);
}

bool TelnetLogger::println(unsigned int number, int base) {
  if (!checkClient()) {
    return false;
  }

  return client.println(number, base);
}

bool TelnetLogger::println(void) {
  if (!checkClient()) {
    return false;
  }

  return client.println();
}
bool TelnetLogger::println(const Printable &p) {
  if (!checkClient()) {
    return false;
  }

  return client.println(p);
}

bool TelnetLogger::printf(const char *format, ...) {
  va_list args;
  if (!checkClient()) {
    return false;
  }

  va_start(args, format);
  auto res = client.printf(format, args);
  va_end(args);
  return res;
}

