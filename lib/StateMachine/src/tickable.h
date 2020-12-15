#ifndef tickable_h_
#define tickable_h_

class Tickable {
  public:
    virtual void tick() = 0;
    virtual void setUp() = 0;
};

#endif
