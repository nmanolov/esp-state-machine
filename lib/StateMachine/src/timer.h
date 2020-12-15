#ifndef timer_h_
#define timer_h_

#include "tickable.h"
#include <functional>

typedef std::function<void ()> Handler;

class AbstractTicker : public Tickable {
  public:
    AbstractTicker(Handler = nullptr);
    void setUp();
    void tick() = 0;
    void setHandler(Handler);
  protected:
    Handler handler;
};

class Timer : public AbstractTicker {
  public:
    Timer(unsigned = 0, Handler = nullptr);
    void tick() override;
    void reset(const unsigned = 0);
  private:
    unsigned timeout;
    unsigned long lastTriggerTime;
};

class Ticker : public AbstractTicker {
  public:
    Ticker(Handler = nullptr);
    void tick() override;
    void reset(bool);
  private:
    bool shouldTick;
};  

#endif
