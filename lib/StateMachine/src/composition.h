#ifndef composition_h_
#define composition_h_

#include "tickable.h"

#include <list>

class Composition : public Tickable {
  private:
    std::list<Tickable *> composed;
  public:
    Composition(const std::initializer_list<Tickable *> tickables);
    void tick();
    void setUp();
};

#endif
