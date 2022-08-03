#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include "BuckConverter.h"
#include "MovingAverageFloat.h"
#include "NewPing.h"
#include <EEPROM.h>

#define ledPin 2

#define phDown 32
#define tdsUp 34
#define water 35
#define mixer 33

float
voltageBAT,
currentLOAD,
socBat,
dt;

int pumpMode[] = {phDown, tdsUp, mixer, water};
int indexComma, len, cond, totalLevel = 27;

String
command,
arrCmd[3],
readInput = "all,1";
unsigned long timerBat;

void setup()
{
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(pumpMode[i], OUTPUT);
    digitalWrite(pumpMode[i], HIGH);
  }
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  initBuck();
  timerBat = millis();
}

// run actuators
void pump(String readInput, int state) {
  if (readInput == "ph down") {
    digitalWrite(phDown, state);
//    digitalWrite(mixer, state);
  }
  else if (readInput == "tds up") {
    digitalWrite(tdsUp, state);
//    digitalWrite(mixer, state);
  }
  else if (readInput == "water") {
    digitalWrite(water, state);
    digitalWrite(mixer, state);
  }
  else if (readInput == "led") {
    digitalWrite(ledPin, state);
  }
  else if (readInput == "mixer") {
    digitalWrite(mixer, state);
  }
  else if (readInput == "all") {
    for (int i = 0; i < 3; i++) {
      digitalWrite(pumpMode[i], state);
    }
  }
}

void getInput() {
  int countCmd = 0;
  readInput = Serial.readString();
  readInput.trim();

  // Split the string into substrings
  while (true)
  {
    int index = readInput.indexOf(';');
    if (index == -1) // No space found
    {
      arrCmd[countCmd++] = readInput;
      break;
    }
    arrCmd[countCmd++] = readInput.substring(0, index);
    readInput = readInput.substring(index + 1);
  }

  for (int i = 0; i < countCmd; i++)
  {
    indexComma = arrCmd[i].lastIndexOf(',');
    len = arrCmd[i].length();
    command = arrCmd[i].substring(0, indexComma);
    cond = arrCmd[i].substring(indexComma + 1, len).toInt();

    pump(command, cond);
  }
}

void loop()
{
  // get input command
  if (Serial.available() > 1) {
    getInput();
  }

  if (readInput == "monitor");
  else if (readInput == "all,1") {
    return;
  }

  currentLOAD = readCurrentLOAD();
  voltageBAT  = readVoltageBat();

  dt = millis() - timerBat; //Time in second
  dt = dt / 1000;
  socBat = readSOC(currentLOAD, dt);
  timerBat = millis();
  relay(socBat);
}
