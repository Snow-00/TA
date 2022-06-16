#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include <EEPROM.h>

#define phPin A0          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin A1

#define phUp 4
#define phDown 5
#define tdsUp 6
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
  gravityTds.setAref(5);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  pinMode(phUp, OUTPUT);
  pinMode(phDown, OUTPUT);
  pinMode(tdsUp, OUTPUT);
  pinMode(tdsDown, OUTPUT);

  timer = millis ();
}

float readPh() {
  voltage = analogRead(phPin) / 1024.0 * 5000; // read the voltage
  phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
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
    digitalWrite(phUp, HIGH);
  }
  else if (readInput == "ph down\n") {
    digitalWrite(phDown, HIGH);
  }
  else if (readInput == "tds up\n") {
    digitalWrite(tdsUp, HIGH);
  }
  else if (readInput == "tds down\n") {
    digitalWrite(tdsDown, HIGH);
  }
  else if (readInput == "t\n") {
    for (int i = 0; i < 4; i++) {
      digitalWrite(pumpMode[i], LOW);
    }
  }
}

void loop()
{
  if (Serial.available() > 1) {
    readInput = Serial.readString();
    pump(readInput);
  }

  if (readInput == "t\n") {
    return;
  }

  timer = millis ();
  phValue = readPh();
  tdsValue = readTds();
  if ((curr1 >= phValue + thresholdPH || curr1 <= phValue - thresholdPH) || (curr2 >= tdsValue + thresholdTDS || curr2 <= tdsValue - thresholdTDS)) {
    Serial.print(timer);
    Serial.print(",");
    Serial.print(phValue, 2);
    Serial.print(",");
    Serial.println(tdsValue, 2);
    curr1 = phValue;
    curr2 = tdsValue;
  }
}
