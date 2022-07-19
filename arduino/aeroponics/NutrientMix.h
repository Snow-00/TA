#ifndef NUTRIENT_MIX_H
#define NUTRIENT_MIX_H

#include "Arduino.h"
#include "DFRobot_PH.h"
#include "GravityTDS.h"

#define phPin A9          // the pH meter Analog output is connected with the Arduino’s Analog
#define tdsPin A10
#define pingPin 50

#define phDown 32
#define tdsUp 34
#define water 35
#define mixer 33

void initNutrient();
float readPh();
float readTds();
float readLevel();
void pump(String readInput, int state);

#endif
