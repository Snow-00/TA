#ifndef NUTRIENT_MIX_H
#define NUTRIENT_MIX_H

#include "Arduino.h"
#include "NewPing.h"

#define PING_PIN 50

#define phDown 32
#define tdsUp 34
#define water 35
#define mixer 33

void initNutrient();
float readLevel();
void pump(String readInput, int state);

#endif
