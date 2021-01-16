#include "pitches.h"
#include "melody.h"
#include <Arduino.h>

int melodyTones[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4,
  NOTE_C4,
  0, 0, 0, 0,
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 4, 4,
  4, 4, 2,
  4, 4, 4, 4,
  2, 2,
  1, 1, 1, 1
};
int Sound = D7;
Melody melody(Sound, melodyTones, noteDurations, 17, 1000, true);


void setup() {
  Serial.begin(9600);
  melody.setUp();
  melody.play();
}
void loop() {
  melody.tick();
}
