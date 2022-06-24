#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include <EEPROM.h>

#define phPin A0          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin A1

#define phUp 4
#define phDown 6
#define tdsUp 5
#define tdsDown 7

DFRobot_PH ph;
GravityTDS gravityTds;

float voltage, phValue, tdsValue, temperature = 25;
int pumpMode[] = {phUp, phDown, tdsUp, tdsDown};

String readInput = "t\n";
unsigned long timer;
float curr1, curr2, percent = 0.01;
float thresholdPH = 14 * percent, thresholdTDS = 4000 * percent; //within x% in either direction

void setup()
{
  Serial.begin(9600);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  for (int i = 0; i < 4; i++) {
    pinMode(pumpMode[i], OUTPUT);
    digitalWrite(pumpMode[i], HIGH);
  }
  timer = millis ();
}

float readPh() {
  voltage = analogRead(phPin) / 1024.0 * 5000; // read the voltage
  phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
//  ph.calibration(voltage, temperature);
  return phValue;
}

float readTds() {
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  //  tdsValue = tdsValue / 500;
  return tdsValue;
}

void pump(String readInput) {
  if (readInput == "ph up\n") {
    digitalWrite(phUp, LOW);
  }
  else if (readInput == "ph down\n") {
    digitalWrite(phDown, LOW);
  }
  else if (readInput == "tds up\n") {
    digitalWrite(tdsUp, LOW);
  }
  else if (readInput == "tds down\n") {
    digitalWrite(tdsDown, LOW);
  }
  else if (readInput == "t\n") {
    for (int i = 0; i < 4; i++) {
      digitalWrite(pumpMode[i], HIGH);
    }
  }
}

void loop()
{
  if (Serial.available() > 1) {
    readInput = Serial.readString();
    pump(readInput);
  }
  if (readInput == "monitor");
  else if (readInput == "t\n") {
    return;
  }

  if (millis() - timer > 1000) {
    timer = millis ();
    phValue = readPh();
    tdsValue = readTds();
    Serial.print(phValue, 2);
    Serial.print(",");
    Serial.println(tdsValue, 2);
    curr1 = phValue;
    curr2 = tdsValue;
  }
}
