#include "melody_switches.h"

MelodySwitch::MelodySwitch(AsyncMqttClient & client, const char * pName, Melody & pMelody) : Switch(client, pName), m(pMelody) {
  m.addListener(*this);
}

MelodySwitch::~MelodySwitch() {
  m.removeListener(*this);
}

void MelodySwitch::onChange(const Melody & m) {
  propagateState(getState());
}

PlaybackSwitch::PlaybackSwitch(AsyncMqttClient & client, const char * name, Melody & pMelody) : MelodySwitch(client, name, pMelody) {}

void PlaybackSwitch::turnOn() {
  m.play();
}
void PlaybackSwitch::turnOff() {
  m.pause();
}
bool PlaybackSwitch::getState() const {
  return m.isPlaying();
}

RepeatSwitch::RepeatSwitch(AsyncMqttClient & client, const char * name, Melody & pMelody) : MelodySwitch(client, name, pMelody) {}
void RepeatSwitch::turnOn() {
  if(m.getRepeat()) {
    return;
  }
  m.toggleRepeat();
}

void RepeatSwitch::turnOff() { 
  if(!m.getRepeat()) {
    return;
  }
  m.toggleRepeat();
}

bool RepeatSwitch::getState() const {
  return m.getRepeat();
}

