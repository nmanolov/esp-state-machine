#ifndef melody_h_
#define melody_h_

#include <Arduino.h>
#include "observer.h"
#include "tickable.h"

class Melody : public Tickable, public Observable<Melody> {
  public:
    Melody(int, int *, int *, unsigned, unsigned = 200, bool = false);
    void tick();
    void setUp();

    bool isPlaying() const;
    void play();
    void pause();
    void toggleRepeat();
    bool getRepeat() const;
  private:
    void playNext(unsigned, bool = false);
    const int pin;
    const int * const tones;
    const int * const durations;
    const unsigned int melodyLength;
    const unsigned int tempo;
    bool repeat;
    bool playing;
    unsigned int nextToneIndex;
    unsigned int currentToneEnd;
};

#endif
