#ifndef NUTRIENT_MIX_H
#define NUTRIENT_MIX_H

#include "Arduino.h"
#include "DFRobot_PH.h"
#include "GravityTDS.h"

#define phPin A0          // the pH meter Analog output is connected with the Arduino’s Analog
#define tdsPin A1
#define echoPin 2
#define trigPin 3

#define phDown 4
#define tdsUp 5
#define water 6

void initNutrient();
float readPh();
float readTds();
float readLevel();
void pump(String readInput, int state);

#endif
