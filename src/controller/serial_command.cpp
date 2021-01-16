#include "serial_command.h"
#include <Arduino.h>

Command::Command(Melody & pM) : melody(pM) {}

void Command::tick() {
  if(!Serial.available()) {
    return;
  }
  Serial.readBytesUntil('\n', buffer, 32);

  if(strcmp(buffer, "pause") == 0) {
    melody.pause();
  } else if (strcmp(buffer, "play") == 0) {
    melody.play();
  } else if (strcmp(buffer, "status") == 0) {
    Serial.println(melody.isPlaying());
  } else if (strcmp(buffer, "repeat") == 0) {
    melody.toggleRepeat();
    Serial.println(melody.getRepeat());    
  }
  memset(buffer, 0, sizeof(buffer));
}

void Command::setUp() {}
