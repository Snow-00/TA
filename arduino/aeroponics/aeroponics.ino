#include "NutrientMix.h"
#include "Env.h"
#include "BuckConverter.h"
#include <EEPROM.h>

float 
levelVal, 
humidVal,
tempVal,
voltagePV,
voltageBAT,
currentPV,
currentBAT,
currentLOAD,
socBat,
dt,
chargeVoltage      = 14.4,
voltageThresh= 1.5000;

int PWM;

int 
indexComma, 
len, 
cond;

String 
command, 
arrCmd[3],
readInput;

unsigned long timer, timerBat;

void setup()
{
  Serial.begin(115200);
  initNutrient();
  initEnv();
  initBuck();
  timer = millis();
  timerBat = millis();
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
    driveAct(command, cond);
  }
}

void loop()
{
  if (Serial.available() > 1) {
    getInput();
  }

  currentPV = readCurrentPV();
  currentBAT = readCurrentBAT();
  currentLOAD = readCurrentLOAD();
  
  voltagePV   = readVoltagePV();
  voltageBAT  = readVoltageBat();
  dt = (millis() - timerBat) / 1000; //Time in second
  Serial.println(dt,6);
  socBat = readSOC(currentBAT, currentLOAD, dt);
  timerBat = millis();
  relay(socBat);

  if(voltagePV < voltageBAT + voltageThresh) PWM = 0;
  if(voltageBAT < chargeVoltage) PWM++; 
  if(voltageBAT > chargeVoltage) PWM--;

  PWM = PWM_Modulation(voltagePV, voltageBAT, PWM);
  
  if (millis() - timer > 1000) {
    timer = millis ();
    levelVal = readLevel();

    humidVal = readHumid();
    tempVal = readTemp();
    
    Serial.print(humidVal, 2);
    Serial.print(",");
    Serial.print(tempVal, 2);
    Serial.print(",");
    Serial.print(levelVal, 2);
    Serial.print(",");
    Serial.print(voltagePV, 2);
    Serial.print(", ");
    Serial.print(voltageBAT, 2);
    Serial.print(", ");
    Serial.println(socBat,5);
  }
}
