#ifndef finite_state_machine_h
#define finite_state_machine_h

#include <observer.h>
#include <n_state.h>

template <class Sygnal, class Context = EmptyContext, class OutputSygnal = bool>
class FiniteStateMachine : public NStateMachine<Sygnal, Context>, public CachingObservable<OutputSygnal> {
  public:
    typedef typename NStateMachine<Sygnal, Context>::RawHandler RawHandler;
  private:
    RawHandler finalState;
  public:
    FiniteStateMachine(
      Context & pContext,
      RawHandler pInitialState,
      RawHandler pFinalState,
      const SygnalMap<Sygnal> & pMap
    ) : NStateMachine<Sygnal, Context>(pContext, pInitialState, pMap),
       CachingObservable<OutputSygnal>(0), finalState(pFinalState) {};
    void setUp() override {
      NStateMachine<Sygnal, Context>::setUp();
      CachingObservable<OutputSygnal>::notifyObservers((OutputSygnal)true);
    };
    void handle(const Sygnal & sygnal, void * data) override {
      NStateMachine<Sygnal, Context>::handle(sygnal, data);
      CachingObservable<OutputSygnal>::notifyObservers((OutputSygnal)(NStateMachine<Sygnal, Context>::currentState->handle != finalState));
    };
};
#endif
