#include <Arduino.h>
#include <logger.h>

SerialLogger::~SerialLogger() {}
void SerialLogger::setUp() {
  Serial.begin(9600);
}
void SerialLogger::tick() {}

bool SerialLogger::print(const char *arg) { return Serial.print(arg); }

bool SerialLogger::println(const char *arg) { return Serial.println(arg); }

bool SerialLogger::print(unsigned int number, int base) {
  return Serial.print(number, base);
}

bool SerialLogger::println(char a) { return Serial.println(a); }
bool SerialLogger::println(unsigned int number, int base) {
  return Serial.println(number, base);
}

bool SerialLogger::printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  auto res = Serial.printf(format, args);
  va_end(args);
  return res;
}

bool SerialLogger::println(void) { return Serial.println(); }

bool SerialLogger::println(const Printable &p) { return Serial.println(p); }

ActualLogger::~ActualLogger() {}
void ActualLogger::setUp() {}
void ActualLogger::tick() {}

bool ActualLogger::println(const char *word) {
  for (auto logger : innerLoggers) {
    if (logger->println(word)) {
      return true;
    }
  }
  return false;
}
bool ActualLogger::print(const char *word) {
  for (auto logger : innerLoggers) {
    if (logger->print(word)) {
      return true;
    }
  }
  return false;
}
bool ActualLogger::println(char c) {
  for (auto logger : innerLoggers) {
    if (logger->println(c)) {
      return true;
    }
  }
  return false;
}
bool ActualLogger::print(unsigned int number, int base) {
  for (auto logger : innerLoggers) {
    if (logger->print(number, base)) {
      return true;
    }
  }
  return false;
}

bool ActualLogger::println(unsigned int number, int base) {
  for (auto logger : innerLoggers) {
    if (logger->println(number, base)) {
      return true;
    }
  }
  return false;
}

bool ActualLogger::println(void) {
  for (auto logger : innerLoggers) {
    if (logger->println()) {
      return true;
    }
  }
  return false;
}
bool ActualLogger::println(const Printable &p) {
  for (auto logger : innerLoggers) {
    if (logger->println(p)) {
      return true;
    }
  }
  return false;
}

bool ActualLogger::printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  for (auto logger : innerLoggers) {
    if (logger->printf(format, args)) {
      va_end(args);
      return true;
    }
  }
  va_end(args);
  return false;
}

void ActualLogger::registerLogger(LoggerClass *logger) {
  this->innerLoggers.push_back(logger);
}

ActualLogger Logger;
