#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include "MovingAverageFloat.h"
#include <EEPROM.h>

#define PH_PIN A0          // the pH meter Analog output is connected with the Arduino’s Analog
#define TDS_PIN A1

DFRobot_PH ph;
GravityTDS gravityTds;

MovingAverageFloat <10> filterPH;
MovingAverageFloat <10> filterTDS;

float
levelValue,
voltage,
phValue,
tdsValue,
temperature = 25;

int totalLevel = 27;

unsigned long timer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.flush();
  gravityTds.setPin(TDS_PIN);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  timer = millis();
}

// read pH sensor
float readPh() {
  voltage = analogRead(PH_PIN) / 1024.0 * 5000; // read the voltage
  phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
  phValue = filterPH.add(phValue);
  return phValue;
}

// read tds sensor
float readTds() {
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  tdsValue = filterTDS.add(tdsValue);
  return tdsValue;
}

void loop() {
  // put your main code here, to run repeatedly:
  // print sensor data
  if (millis() - timer > 1000) {
    timer = millis ();
    phValue = readPh();
    tdsValue = readTds();

    Serial.print(phValue, 2);
    Serial.print(",");
    Serial.println(tdsValue, 2);
  }
}
