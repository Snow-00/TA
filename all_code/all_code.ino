#include "NutrientMix.h"
#include "Env.h"
#include "BuckConverter.h"
#include <EEPROM.h>

float 
levelVal, 
phVal, 
tdsVal,
humidVal,
tempVal,
voltagePV,
voltageBAT,
currentPV,
currentBAT,
currentLOAD,
socBat,
dt,
chargeVoltage      = 14.700,
voltageThresh= 1.5000;

int PWM;

int 
indexComma, 
len, 
cond;

String 
command, 
readInput;

unsigned long timer, timerBat;

void setup()
{
  Serial.begin(9600);
  initNutrient();
  initEnv();
  initBuck();
  timer = millis();
  timerBat = millis();
}

void loop()
{
  if (Serial.available() > 1) {
    readInput = Serial.readString();
    indexComma = readInput.lastIndexOf(',');
    len = readInput.length();
    command = readInput.substring(0, indexComma);
    cond = readInput.substring(indexComma + 1, len).toInt();
    
    pump(command, cond);
    driveAct(command, cond);
  }

  currentPV = readCurrentPV();
  currentBAT = readCurrentBAT();
  currentLOAD = readCurrentLOAD();
  
  voltagePV   = readVoltagePV();
  voltageBAT  = readVoltageBat();
  dt = (millis() - timerBat) / 1000; //Time in second
  socBat = readSOC(currentBAT, currentLOAD, dt);
  timerBat = millis();
  relay(socBat);

  if(voltagePV < voltageBAT + voltageThresh) PWM = 0;
  if(voltageBAT < chargeVoltage) PWM++; 
  if(voltageBAT > chargeVoltage) PWM--;

  PWM = PWM_Modulation(voltagePV, voltageBAT, PWM);
  
  if (millis() - timer > 300) {
    timer = millis ();
    phVal = readPh();
    tdsVal = readTds();
    levelVal = readLevel();

    humidVal = readHumid();
    tempVal = readTemp();
    
    Serial.print(phVal, 2);
    Serial.print(",");
    Serial.print(tdsVal, 2);
    Serial.print(",");
    Serial.print(levelVal, 2);
    Serial.print(",");
    Serial.print(humidVal, 2);
    Serial.print(",");
    Serial.print(tempVal, 2);
    Serial.print(",");
    Serial.print(voltagePV, 2);
    Serial.print(", ");
    Serial.print(voltageBAT, 2);
    Serial.print(", ");
    Serial.print(currentPV, 2);
    Serial.print(", ");
    Serial.print(currentBAT, 2);
    Serial.print(", ");
    Serial.print(currentLOAD, 2); 
    Serial.print(", ");
    Serial.println(socBat);
  }
}
