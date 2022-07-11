#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>;

// sensors pin
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define DHTPIN A0
#define DHTPIN2 A1
#define DHTPIN3 A2
#define DHTPIN4 A3
#define DHTTYPE DHT22
#define echoPin 11
#define trigPin 12

// actuator pin
#define R_PWM 10
#define L_PWM 11
#define R_EN 8
#define L_EN 9
#define pinPump 7

DHT dht(DHTPIN, DHTTYPE);
DHT dht2 (DHTPIN2, DHTTYPE);
DHT dht3 (DHTPIN3, DHTTYPE);
DHT dht4 (DHTPIN4, DHTTYPE);

String input = "all,1\n", command;
int indexComma, len, cond, pinAct[] = {R_EN, L_EN, pinPump};
float hValue, hValue2, hValue3, hValue4, avgHumid, avgTemp;
unsigned long timer;
float duration, levelValue;
int totalLevel = 27;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // setup actuator pin
  for (int i = 0; i < 3; i++) {
    pinMode(pinAct[i], OUTPUT);
    digitalWrite(pinAct[i], HIGH);
  }
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  digitalWrite(R_PWM, 0);
  digitalWrite(L_PWM, 0);

  timer = millis();

}

// read level sensor
float readLevel() {
  // sending trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // get the distance value
  duration = pulseIn(echoPin, HIGH);
  levelValue = duration * 0.034 / 2;
  levelValue = totalLevel - levelValue;
  return levelValue;
}

// reading Humidity sensor
float readHumid() {
  hValue = dht.readHumidity();
  hValue2 = dht2.readHumidity();
  hValue3 = dht3.readHumidity();
  hValue4 = dht4.readHumidity();

  avgHumid = (hValue + hValue2 + hValue3 + hValue4) / 4;
  return avgHumid;
}

// reading Temperatur sensor
float readTemp() {
  sensors.requestTemperatures();
  avgTemp = sensors.getTempCByIndex(0);
  return avgTemp;
}

// run the actuator
void driveAct(String cmd, int state) {
  if (cmd == "peltier") {
    digitalWrite(R_PWM, abs(state-1));
    digitalWrite(L_PWM, 0);
  }
  else if (cmd == "pump") {
    digitalWrite(pinPump, state);
  }
  else if (cmd == "all") {
    digitalWrite(R_PWM, abs(state-1));
    digitalWrite(L_PWM, 0);
    digitalWrite(pinPump, state);
  }
}

void loop() {
  // get command input
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
  if (millis() - timer > 300) {
    timer = millis ();
    avgHumid = readHumid();
    avgTemp = readTemp();
    levelValue = readLevel();

    Serial.print(avgTemp, 2);
    Serial.print(",");
    Serial.print(avgHumid, 2);
    Serial.print(",");
    Serial.println(levelValue, 2);
  }
}
