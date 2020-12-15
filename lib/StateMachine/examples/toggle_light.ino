#include "n_state.h"
#include "button.h"

enum OnOff {
  Timeout,
  Initialize,
  Toggle,
  Tick,
};

typedef NStateMachine<OnOff> MyStateMachine;
typedef NState<OnOff> MyState;

MyState * on(MyState*, StateMachineHandle<EmptyContext> &, const OnOff &);
MyState * off(MyState*, StateMachineHandle<EmptyContext> &, const OnOff &);

MyState * on(MyState* self, StateMachineHandle<EmptyContext> & machine, const OnOff & sygnal) {
  switch(sygnal) {
    case Initialize:
      pinMode(LED_BUILTIN, HIGH);
      return self;
    case Toggle:
      return new MyState(&off);
    default:
      return self;
  }
}

MyState * off(MyState* self, StateMachineHandle<EmptyContext> & machine, const OnOff & sygnal) {
  switch(sygnal) {
    case Initialize:
      pinMode(LED_BUILTIN, LOW);
      return self;
    case Toggle:
      return new MyState(&on);
    default:
      return self;
  }
}

EmptyContext context;
MyStateMachine toggly(
  context,
  &on,
  {Initialize, Timeout, Tick});

Button toggleButton(D6, [](){
  Serial.println("button pressed");
  toggly.dispatch(Toggle, nullptr);
});

void setup() {
  toggly.setUp();
  toggleButton.setUp();
}

void loop() {
  toggly.tick();
  toggleButton.tick();
}

