#include "n_state.h"


enum OnOff {
  Timeout,
  Initialize,
  Tick,
};

typedef NStateMachine<OnOff> MyStateMachine;
typedef NState<OnOff> MyState;

MyState * on(MyState*, StateMachineHandle<EmptyContext> &, const OnOff &);
MyState * off(MyState*, StateMachineHandle<EmptyContext> &, const OnOff &);

MyState * on(MyState* self, StateMachineHandle<EmptyContext> & machine, const OnOff & sygnal) {
  switch(sygnal) {
    case Initialize:
      machine.setStateTimeout(3000);
      pinMode(LED_BUILTIN, HIGH);
      return self;
    case Timeout:
      return new MyState(&off);
    default:
      return self;
  }
}

MyState * off(MyState* self, StateMachineHandle<EmptyContext> & machine, const OnOff & sygnal) {
  switch(sygnal) {
    case Initialize:
      machine.setStateTimeout(3000);
      pinMode(LED_BUILTIN, LOW);
      return self;
    case Timeout:
      return new MyState(&on);
    default:
      return self;
  }
}

EmptyContext context;
MyStateMachine blinky(
  context,
  &on,
  {Initialize, Timeout, Tick});


void setup() {
  blinky.setUp();
}

void loop() {
  blinky.tick();
}

