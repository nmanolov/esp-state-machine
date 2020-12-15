#ifndef command_h_
#define command_h_

#include "melody.h"
#include "tickable.h"

class Command : public Tickable {
  public:
    Command(Melody &);
    void tick();
    void setUp();
  private:
    char buffer[32];
    Melody & melody;
};

#endif
