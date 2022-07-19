#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include "BuckConverter.h"
#include <EEPROM.h>

#define phPin A9          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin A10
#define pingPin 50

#define ledPin 2

#define phDown 32
#define tdsUp 34
#define water 35
#define mixer 33

DFRobot_PH ph;
GravityTDS gravityTds;

float
duration,
levelValue,
prevLevel,
voltage,
phValue,
tdsValue,
temperature = 25,
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
unsigned long timer, timerBat;

void setup()
{
  Serial.begin(115200);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  for (int i = 0; i < 4; i++) {
    pinMode(pumpMode[i], OUTPUT);
    digitalWrite(pumpMode[i], HIGH);
  }
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  initBuck();
  timer = millis ();
  timerBat = millis();
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
  if (prevLevel == 0) {
    prevLevel = levelValue;
    return levelValue;
  }

  if (abs(levelValue - prevLevel) > 1.5) {
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
  else if (readInput == "led") {
    digitalWrite(ledPin, state);
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
  
  // print sensor data
  if (millis() - timer > 300) {
    timer = millis ();
    phValue = readPh();
    tdsValue = readTds();
    levelValue = readLevel();

    Serial.print(phValue, 2);
    Serial.print(",");
    Serial.print(tdsValue, 2);
    Serial.print(",");
    Serial.print(levelValue, 2);
    Serial.print(",");
    Serial.print(voltageBAT, 2);
    Serial.print(",");
    Serial.print(currentLOAD, 2); 
    Serial.print(",");
    Serial.println(socBat,5);
  }
}
