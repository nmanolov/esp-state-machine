#ifndef output_pin_h
#define output_pin_h

#include <tickable.h>
#include <observer.h>

class OutputPin : public Tickable, public Observable<OutputPin> {
public:
  OutputPin(const unsigned, const bool = false);
  void tick();
  void setUp();
  void turnOn();
  void turnOff();
  bool getState() const;

private:
  unsigned pin;
  bool invert;
};

#endif
