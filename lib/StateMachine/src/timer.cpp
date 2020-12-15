
#include "timer.h"
#include <Arduino.h>

AbstractTicker::AbstractTicker(Handler pHandler) : handler(pHandler) {}
void AbstractTicker::setUp() {}

void AbstractTicker::setHandler(Handler pHandler) {
  handler = pHandler;
}

Timer::Timer(unsigned pTimeout, Handler pHandler) :
  AbstractTicker(pHandler),
  timeout(pTimeout),
  lastTriggerTime(millis()) { }

void Timer::reset(const unsigned pTimeout) {
  timeout = pTimeout;
  lastTriggerTime = millis();
}

void Timer::tick() {
  if(timeout == 0 || handler == nullptr) {
    return;
  }
  unsigned long currentTime = millis();
  if(currentTime - lastTriggerTime > timeout) {
    handler();
    lastTriggerTime = currentTime;
  }
}

Ticker::Ticker(Handler pHandler):
  AbstractTicker(pHandler),
  shouldTick(false) {}

void Ticker::tick() {
  if(handler == nullptr || !shouldTick) {
    return;
  }
  handler();
}

void Ticker::reset(bool pShouldTick) {
  shouldTick = pShouldTick;
}
