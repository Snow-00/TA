#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include <EEPROM.h>

#define phPin A0          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin A1
#define echoPin 2
#define pingPin 3

#define phDown 32
#define tdsUp 34
#define water 35
#define mixer 33

DFRobot_PH ph;
GravityTDS gravityTds;

float duration, levelValue, voltage, phValue, tdsValue, temperature = 25;
int pumpMode[] = {phDown, tdsUp, water};
int indexComma, len, cond, totalLevel = 27;

String command, readInput = "all,1\n";
unsigned long timer;

void setup()
{
  Serial.begin(115200);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  for (int i = 0; i < 3; i++) {
    pinMode(pumpMode[i], OUTPUT);
    digitalWrite(pumpMode[i], HIGH);
  }
  timer = millis ();
}

// read pH sensor
float readPh() {
  voltage = analogRead(phPin) / 1024.0 * 5000; // read the voltage
  phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
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
  else if (readInput == "all") {
    for (int i = 0; i < 2; i++) {
      digitalWrite(pumpMode[i], state);
    }
  }
}

void loop()
{
  // get input command
  if (Serial.available() > 1) {
    readInput = Serial.readString();
//    Serial.println(readInput);/
    indexComma = readInput.lastIndexOf(',');
    len = readInput.length();
    command = readInput.substring(0, indexComma);
    cond = readInput.substring(indexComma + 1, len).toInt();
    
    pump(command, cond);
  }
  
  if (readInput == "monitor");
  else if (readInput == "all,1\n") {
    return;
  }

  // print sensor data
  if (millis() - timer > 1000) {
    timer = millis ();
    phValue = readPh();
    tdsValue = readTds();
    levelValue = readLevel();
    
    Serial.print(phValue, 2);
    Serial.print(",");
    Serial.print(tdsValue, 2);
    Serial.print(",");
    Serial.println(levelValue, 2);
  }
}
