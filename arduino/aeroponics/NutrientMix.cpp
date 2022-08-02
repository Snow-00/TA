#include "NutrientMix.h"
#include <EEPROM.h>

NewPing sonar(PING_PIN, PING_PIN, 500);

float 
duration, 
levelValue;

int pumpMode[] = {phDown, tdsUp, mixer, water};

const int 
totalLevel  = 27, 
temperature = 25;

void initNutrient() {
  for (int i = 0; i < 4; i++) {
    pinMode(pumpMode[i], OUTPUT);
    digitalWrite(pumpMode[i], HIGH);
  }
}

// read level sensor
float readLevel() {
  levelValue = sonar.ping_cm();
  levelValue = totalLevel - levelValue;
  return levelValue;
}

// run actuators
void pump(String readInput, int state) {
  if (readInput == "ph down") {
    digitalWrite(phDown, state);
    digitalWrite(mixer, state);
  }
  else if (readInput == "tds up") {
    digitalWrite(tdsUp, state);
    digitalWrite(mixer, state);
  }
  else if (readInput == "water") {
    digitalWrite(water, state);
    digitalWrite(mixer, state);
  }
  else if (readInput == "mixer") {
    digitalWrite(mixer, state);
  }
  else if (readInput == "all ntr") {
    for (int i = 0; i < 3; i++) {
      digitalWrite(pumpMode[i], state);
    }
  }
}
