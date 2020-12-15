#include "composition.h"

Composition::Composition(const std::initializer_list<Tickable *> tickables) : composed(tickables) {}

void Composition::tick() {
  for(auto c: composed) {
    c->tick();
  }
}

void Composition::setUp() {
  for(auto c: composed) {
    c->setUp();
  }
}
