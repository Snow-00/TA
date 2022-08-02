#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>;
#include <MovingAverageFloat.h>;

// sensors pin
#define ONE_WIRE_BUS 30
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
MovingAverageFloat <10> filterHumid;
MovingAverageFloat <10> filterTemp;

#define pinLed  2 // for boards with CHANGEBLE input pins
#define DHTPIN A0
#define DHTPIN2 A1
#define DHTPIN3 A2
#define DHTPIN4 A3
#define DHTTYPE DHT22
#define pingPin 48


// actuator pin
#define R_PWM 4
#define L_PWM 5
#define R_EN 6
#define L_EN 7
#define pinPump 3

DHT dht(DHTPIN, DHTTYPE);
DHT dht2 (DHTPIN2, DHTTYPE);
DHT dht3 (DHTPIN3, DHTTYPE);
DHT dht4 (DHTPIN4, DHTTYPE);

String input = "all,1\n", command;
int indexComma, len, cond, pinAct[] = {R_EN, L_EN, pinPump,pinLed};
float hValue, hValue2, hValue3, hValue4, avgHumid, avgTemp;
unsigned long timer;
float duration, levelValue;
int totalLevel = 27;
int deviceCount = 0;
float tempC;



void setup() {
  // put your setup code here, to run once:
  //  dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE)
  Serial.begin(9600);
  dht.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();
  sensors.begin();


  //  dimmer.setPower(LEDState);
  deviceCount = sensors.getDeviceCount();
//   setup actuator pin
//    Serial.print(deviceCount, DEC);
//    Serial.println(" devices.");
//    Serial.println("");
  for (int i = 0; i < 3; i++) {
    pinMode(pinAct[i], OUTPUT);
    digitalWrite(pinAct[i], HIGH);
  }
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, LOW);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  digitalWrite(R_PWM, 0);
  digitalWrite(L_PWM, 0);
  timer = millis();


}
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
// read level sensor

// reading Humidity sensor
float readHumid() {
  hValue = dht.readHumidity();
  hValue2 = dht2.readHumidity();
  hValue3 = dht3.readHumidity();
  hValue4 = dht4.readHumidity();

  avgHumid = (hValue + hValue2 + hValue3 + hValue4) / 4;
  avgHumid = filterHumid.add(avgHumid);
  return avgHumid;
}

// reading Temperatur sensor
float readTemp() {
  sensors.requestTemperatures();
  float totalTemp = 0;
  // Display temperature from each sensor
  for (int i = 0;  i < deviceCount;  i++)
  {
    tempC = sensors.getTempCByIndex(i);
    totalTemp = totalTemp + tempC;
  }
  float avgTemp = totalTemp / deviceCount;
  avgTemp = filterTemp.add(avgTemp);
  return avgTemp;
}

// run the actuator
void driveAct(String cmd, int state) {
  if (cmd == "peltier") {
    digitalWrite(R_PWM, abs(state - 1));
    digitalWrite(L_PWM, 0);
  }
  else if (cmd == "pump") {
    digitalWrite(pinPump, state);
  }
  else if (cmd == "led"){
    digitalWrite(pinLed, state);
  }
  else if (cmd == "all") {
    digitalWrite(R_PWM, abs(state - 1));
    digitalWrite(L_PWM, 0);
    digitalWrite(pinPump, state);
    digitalWrite(pinLed, state);
  }
}

 

void loop() {
  if (Serial.available() > 1 ) {
    input = Serial.readString();
    indexComma = input.lastIndexOf(',');
    len = input.length();
    command = input.substring(0, indexComma);
    cond = input.substring(indexComma + 1, len).toInt();

    driveAct(command, cond);
  }

  if (input == "monitor,0\n");
  else if (input == "all,1\n") {
    return;
  }
  // print sensors value
  if (millis() - timer > 100) {
    timer = millis ();
    avgHumid = readHumid();
    avgTemp = readTemp();
    levelValue = readLevel();
    
    Serial.print(avgHumid, 2);
    Serial.print(",");
    Serial.print(avgTemp, 2);
    Serial.print(",");
    Serial.println(levelValue, 2);
  }


}
