#ifndef button_h_
#define button_h_

#include "tickable.h"
#include <functional>

typedef std::function<void()> PressHandler;

class Button : public Tickable {
  public:
    Button(unsigned pin, PressHandler handler, unsigned debounce = 20);
    void tick();
    void setUp();
  private:
    const unsigned pin;
    PressHandler handler; 
    int lastReading = 0;
    unsigned long time = 0;
    unsigned debounceTime;
};

#endif
