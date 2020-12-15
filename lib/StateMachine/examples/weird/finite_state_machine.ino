
#include <n_state.h>
#include "button.h"
#include "finite_state_machine.h"
#include <Arduino.h>

typedef FiniteStateMachine<int, int, bool> Testee;
typedef NState<int, int> TState;
TState * ton(TState * self, StateMachineHandle<int> & handle, const int & sygnal);
TState * toff(TState * self, StateMachineHandle<int> & handle, const int & sygnal);
TState * ton(TState * self, StateMachineHandle<int> & handle, const int & sygnal) {
  switch(sygnal) {
    case 0:
      handle.setStateTimeout(1000);
      handle.forceLoop();
      Serial.println("Enterring ON");
      return self;
    case 1:
      return new TState(&toff);
    case 2:
      Serial.println("Loop On");
    default:
      return self;
  }
}

TState * toff(TState * self, StateMachineHandle<int> & handle, const int & sygnal) {
  switch(sygnal) {
    case 0:
      handle.setStateTimeout(1000);
      Serial.println("Enterring OFF");
      return self;
    case 1:
      return new TState(&ton);
    case 2:
      Serial.println("Loop Off");
    default:
      return self;
  }
}
int x = 1;
Testee machine(x, &ton, &toff, {0, 1, 2});

class BoolObserver : public Observer<bool> {
  Handler handler;
  public:
    BoolObserver(Handler h) : handler(h) {}
    void onChange(const bool & change) override {
      handler();
    }
};

BoolObserver machineListener([]() {
  Serial.println("Change");
});

void setup() {
  Serial.begin(9600);
  machine.addListener(machineListener);
  machine.setUp();
}
void loop() {
  machine.tick();
}
