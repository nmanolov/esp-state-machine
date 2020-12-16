
#include <Arduino.h>
#include "queue.h"
#include "timer.h"

#ifndef nstate_h_
#define nstate_h_

class EmptyContext {};

template<class Context = EmptyContext>
class StateMachineHandle {
public:
  typedef std::function<void(const unsigned)> TimeoutSetter;
  typedef std::function<void()> LooperSetter;
  StateMachineHandle(
    Context & pState,
    TimeoutSetter pSetTickTime,
    TimeoutSetter pSetStateTimeout,
    LooperSetter pSetLoop) : state(pState),
      setStateTickTime(pSetTickTime),
      setStateTimeout(pSetStateTimeout),
      forceLoop(pSetLoop) { }
  Context & state;
  TimeoutSetter setStateTickTime;
  TimeoutSetter setStateTimeout;
  LooperSetter forceLoop;
};

template<class Sygnal>
struct SygnalMap {
  const Sygnal initialize;
  const Sygnal timeout;
  const Sygnal tick;
  // const Sygnal terminate;
};

template <class Sygnal, class Context = EmptyContext>
class NState {
public:
  typedef NState* (*RawHandler)(NState *, StateMachineHandle<Context> &, const Sygnal &);
  
  NState(RawHandler h) : handle(h){ };

  RawHandler handle;
};

template <class Sygnal, class Context = EmptyContext>
class NStateMachine : public Tickable {
protected:
  Context & context;
private:
  Queue<Sygnal> eventQueue;
  Timer timer;
  Timer ticker;
  Ticker ticker2;
protected:
  NState<Sygnal, Context> * const initialState;
  NState<Sygnal, Context> * currentState;
  const SygnalMap<Sygnal> sygnals;
private:
  StateMachineHandle<Context> machine;
protected:
  virtual void handle(const Sygnal &, void *);

public:
  typedef typename NState<Sygnal, Context>::RawHandler RawHandler;
  NStateMachine(Context &,
                RawHandler,
                const SygnalMap<Sygnal> &);
  NStateMachine(RawHandler,
                const SygnalMap<Sygnal> &);
  void dispatch(const Sygnal &, void *);
  void setUp();
  void tick();
private:
  void resetTimers();
};

template<class Sygnal, class Context>
NStateMachine<Sygnal, Context>::NStateMachine(
  Context & pContext,
  RawHandler pInitialState,
  const SygnalMap<Sygnal> &pMap
) :
  context(pContext),
  initialState(new NState<Sygnal,Context>(pInitialState)),
  currentState(initialState),
  sygnals(pMap),
  machine(
    context,
    [this](const unsigned tick) { ticker.reset(tick); },
    [this](const unsigned timeout) { timer.reset(timeout); },
    [this]() { ticker2.reset(true); }
  ) {
    timer.setHandler([this] {
      dispatch(sygnals.timeout, nullptr);
    });
    ticker.setHandler([this] {
      dispatch(sygnals.tick, nullptr);
    });
    ticker2.setHandler([this] {
      dispatch(sygnals.tick, nullptr);
    });
  }
      
template<class Sygnal, class Context>
void NStateMachine<Sygnal, Context>::setUp() {
  currentState = initialState;
  dispatch(sygnals.initialize, nullptr);
}

template<class Sygnal, class Context>
void NStateMachine<Sygnal, Context>::dispatch(const Sygnal & sygnal, void * data) {
  if(sygnal == sygnals.timeout && eventQueue.end()->sygnal() == sygnal) {
    return;
  }
  eventQueue.push(sygnal, data);
}

template<class Sygnal, class Context>
void NStateMachine<Sygnal, Context>::tick() {
  ticker2.tick();
  ticker.tick();
  timer.tick();

  while(!eventQueue.isEmpty()) {
    QueueNode<Sygnal> node = eventQueue.pop();
    handle(node.sygnal(), node.data());
  }
}

template<class Sygnal, class Context>
void NStateMachine<Sygnal, Context>::handle(const Sygnal & sygnal, void * data) {
  if(sygnal == sygnals.initialize) {
    resetTimers();
  }
  
  auto nextState = currentState->handle(currentState, machine, sygnal);
  if (nextState == nullptr || nextState->handle == nullptr) {
    return;
  }
  if (nextState == currentState) {
    return;
  }

  if(currentState != initialState) {
    delete currentState;
  }
  currentState = nextState;
  dispatch(sygnals.initialize, nullptr);
}

template<class Sygnal, class Context>
void NStateMachine<Sygnal, Context>::resetTimers() {
  timer.reset(0);
  ticker.reset(0);
  ticker2.reset(false);

  eventQueue.erase(
    std::remove_if(
      eventQueue.begin(),
      eventQueue.end(),
      [this](QueueNode<Sygnal> node) {
        return node.sygnal() == sygnals.timeout
          || node.sygnal() == sygnals.tick;
      }),
    eventQueue.end());
}

#endif
