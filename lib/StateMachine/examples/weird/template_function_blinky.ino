
#include <n_state.h>
#include "button.h"
#include <Arduino.h>

enum OnOff {
  Timeout,
  Initialize,
  ButtonPress,
  Tick,
};

typedef NStateMachine<OnOff> MyStateMachine;
typedef NState<OnOff> MyState;

template<int VALUE = HIGH, int Timeout = 1000>
MyState * state(MyState * self, StateMachineHandle<EmptyContext> & machine, const OnOff & sygnal) {
    switch(sygnal) {
    case Initialize:
      machine.setStateTimeout(Timeout);
      pinMode(LED_BUILTIN, VALUE);
      pinMode(LED_BUILTIN_AUX, VALUE);
      return self;
    case ButtonPress:
    case Timeout:
      return new MyState(&remaining<!VALUE, Timeout>);
    default:
      return self;
  }
}

EmptyContext context;
MyStateMachine lightOnOff(
  context,
  &state<HIGH, 1500>,
  {Initialize, Timeout, Tick});

Button onOffButton(D6, [](){
  Serial.println("button pressed");
  lightOnOff.dispatch(ButtonPress, nullptr);
});

void setup() {
  Serial.begin(9600);
  lightOnOff.setUp();
  onOffButton.setUp();
}
void loop() {
  lightOnOff.tick();
  onOffButton.tick();
}
