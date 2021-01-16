#include "melody.h"

Melody::Melody(int pPin, int * pTones, int * pDurations, unsigned int pMelodyLength, unsigned int pTempo, bool pRepeat)
  : pin(pPin), tones(pTones), durations(pDurations), melodyLength(pMelodyLength), tempo(pTempo), repeat(pRepeat),
  playing(false), nextToneIndex(0), currentToneEnd(0)
{ }

void Melody::setUp() {
  pinMode(pin, OUTPUT);
}

void Melody::tick() {
  if (!playing) {
    return;
  }
  auto now = millis();
  if(now < currentToneEnd) {
    return;
  }
  playNext(now);
}

bool Melody::isPlaying() const {
  return playing;
}

void Melody::play() {
  if(playing) {
    return;
  }
  playing = true;
  notifyObservers(*this);
  playNext(millis(), true);
}

void Melody::playNext(unsigned now, bool forcePlayFromBegin) {
  if(nextToneIndex == melodyLength) {
    nextToneIndex = 0;
    if(!repeat && !forcePlayFromBegin) {
      playing = false;
      notifyObservers(*this);
      noTone(pin);
      return;
    }
  }
  const unsigned duration = tempo / durations[nextToneIndex] ;
  currentToneEnd = now + duration;
  tone(pin, tones[nextToneIndex], duration * 0.75);
  nextToneIndex++;
}

void Melody::pause() {
  if(!playing) {
    return;
  }
  playing = false;
  currentToneEnd = 0;
  notifyObservers(*this);
}

void Melody::toggleRepeat() {
  repeat = !repeat;
  notifyObservers(*this);
}

bool Melody::getRepeat() const {
  return repeat;
}
