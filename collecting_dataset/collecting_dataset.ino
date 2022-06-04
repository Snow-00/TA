#include <Wire.h>
#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include "GravityTDS.h"

#define phPin A0          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin A1
#define out1 2  // power
#define out2 3  // ground
#define phUp 6
#define phDown 5
#define tdsUp 4
#define tdsDown 7

GravityTDS gravityTds;

int phval = 0;
float phValue;
unsigned long int avgval;
int buffer_arr[10], temp;
float calibration_value = 21.34 - 0.2;

float temperature = 25, tdsValue;
int tdsCalibrate = -223;   // -240
int pumpMode[] = {phUp, phDown, tdsUp, tdsDown};

String readInput;
uint32_t timer;
int read_sens = 0;

void setup()
{
  Serial.begin(9600);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization

  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(phUp, OUTPUT);
  pinMode(phDown, OUTPUT);
  pinMode(tdsUp, OUTPUT);
  pinMode(tdsDown, OUTPUT);

  timer = millis ();
}

float readPH() {
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A0);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  float ph_act = -5.70 * volt + calibration_value;
  float ph = ph_act - 0;         //come from difference between sensor read and measurement tools read
  return ph;
}

float readTds() {
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue() + tdsCalibrate;  // then get the value
  //  tdsValue = tdsValue / 500;
  return tdsValue;
}

int trigger(int x) {
  digitalWrite(out1, x);
  digitalWrite(out2, LOW);
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
  if (millis() - timer > 1000) {
    timer = millis ();
    if (read_sens == 0) {
      trigger(0);
      read_sens = 1;
      phValue = readPH();
      Serial.print("pH: ");
      Serial.println(phValue, 2);
      trigger(1);
    }
    else {
      read_sens = 0;
      tdsValue = readTds();
      Serial.print("Tds: ");
      Serial.print(tdsValue, 2);
      Serial.println("ppm");
    }
  }

  readInput = Serial.readString();
  if (readInput) {
    Serial.println(readInput);
    pump(readInput);
  }
}
