#include "NutrientMix.h"
#include "Env.h"
#include "BuckConverter.h"
#include <EEPROM.h>

// converter sensors
#define v_PV  A0
#define i_PV  A1
#define v_Bat A2
#define i_Bat A3
#define i_Load A4

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
chgVoltage      = 13.000,
voltageThreshold= 1.5000;

int 
indexComma, 
len, 
cond;

String 
command, 
readInput;

unsigned long timer;

void setup()
{
  Serial.begin(9600);
  initNutrient();
  initEnv();
  initBuck();
  timer = millis();
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

  if (millis() - timer > 300) {
    timer = millis ();
    phVal = readPh();
    tdsVal = readTds();
    levelVal = readLevel();

    humidVal = readHumid();
    tempVal = readTemp();

    voltagePV   = readVoltage(v_PV);
    currentPV   = readCurrent(i_PV);
    voltageBAT  = readVoltage(v_Bat);
    currentBAT  = readCurrent(i_Bat);
    currentLOAD = readCurrent(i_Load);
    socBat = readSOC(currentBAT, currentLOAD);
    relay(socBat);

    if(voltagePV < voltageBAT + voltageThreshold) OCR4C = 0;
    if(voltageBAT < chgVoltage) OCR4C++; 
    if(voltageBAT > chgVoltage) OCR4C--;

    OCR4C = constrain(OCR4C,15,300);
    
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
