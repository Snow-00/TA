#include "NutrientMix.h"
#include <EEPROM.h>

DFRobot_PH ph;
GravityTDS gravityTds;

float 
duration, 
levelValue, 
prevLevel,
vPH, 
phValue, 
tdsValue;

int pumpMode[] = {phDown, tdsUp, mixer, water};

const int 
totalLevel  = 27, 
temperature = 25;

void initNutrient() {
  gravityTds.setPin(tdsPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  for (int i = 0; i < 4; i++) {
    pinMode(pumpMode[i], OUTPUT);
    digitalWrite(pumpMode[i], HIGH);
  }
}

// read pH sensor
float readPh() {
  vPH = analogRead(phPin) / 1024.0 * 5000; // read the voltage
  phValue = ph.readPH(vPH, temperature); // convert voltage to pH with temperature compensation
//  ph.calibration(voltage, temperature);
  return phValue;
}

// read tds sensor
float readTds() {
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  //  tdsValue = tdsValue / 500;
  return tdsValue;
}

// read level sensor
float readLevel() {
  // sending trigger pulse
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  levelValue = duration * 0.017;
  levelValue = totalLevel - levelValue;
  if (prevLevel == 0) {
    prevLevel = levelValue;
    return levelValue;
  }
  
  if (abs(levelValue-prevLevel) > 1.5) {
    levelValue = prevLevel;
  }
  else {
    prevLevel = levelValue;
  }
  return levelValue;
}

// run actuators
void pump(String readInput, int state) {
  if (readInput == "ph down") {
    digitalWrite(phDown, state);
  }
  else if (readInput == "tds up") {
    digitalWrite(tdsUp, state);
  }
  else if (readInput == "water") {
    digitalWrite(water, state);
  }
  else if (readInput == "mixer") {
    digitalWrite(mixer, state);
  }
  else if (readInput == "all") {
    for (int i = 0; i < 4; i++) {
      digitalWrite(pumpMode[i], state);
    }
  }
}
