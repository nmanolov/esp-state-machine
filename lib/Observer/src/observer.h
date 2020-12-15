
#ifndef observer_h_
#define observer_h_

#include <list>

template <class Model>
class Observer {
  public:
    virtual void onChange(const Model & model) = 0;
};

template <class Model>
class Observable {
  public:
    Observable();
    void addListener(Observer<Model> &);
    void removeListener(Observer<Model> &);
  protected:
    virtual void notifyObservers(const Model &);
  private:
    std::list<Observer<Model> *> observers;
};

template<class Model>
Observable<Model>::Observable() { }

template<class Model>
void Observable<Model>::addListener(Observer<Model> & observer) {
  observers.push_back(&observer);
}

template<class Model>
void Observable<Model>::removeListener(Observer<Model> & observer) {
  observers.remove(&observer);
}

template<class Model>
void Observable<Model>::notifyObservers(const Model & model) {
  for(auto observer: observers) {
    observer->onChange(model);
  }
}

template<class Model>
class CachingObservable : public Observable<Model> {
  private:
    Model lastNotificationValue;
    bool hasNotified;
  public:
    CachingObservable(const Model &);
    void notifyObservers(const Model &) override;
};

template<class Model>
CachingObservable<Model>::CachingObservable(const Model & initialValue) : 
  lastNotificationValue(initialValue), hasNotified(false) { }

template<class Model>
void CachingObservable<Model>::notifyObservers(const Model & model) {
  if (hasNotified && model == lastNotificationValue) {
    return;
  }
  hasNotified = true;
  lastNotificationValue = model;
  Observable<Model>::notifyObservers(model);
}

#endif
