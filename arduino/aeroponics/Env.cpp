#include "Env.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DHT dht(DHTPIN, DHTTYPE);
DHT dht2 (DHTPIN2, DHTTYPE);
DHT dht3 (DHTPIN3, DHTTYPE);
DHT dht4 (DHTPIN4, DHTTYPE);

int pinAct[] = {R_EN, L_EN, pinPump, ledPin};

float 
hValue, 
hValue2, 
hValue3, 
hValue4, 
avgHumid, 
avgTemp;

void initEnv() {
  dht.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();

  // setup actuator pin
  for (int i = 0; i < 4; i++) {
    pinMode(pinAct[i], OUTPUT);
    digitalWrite(pinAct[i], HIGH);
  }
  
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  digitalWrite(R_PWM, 0);
  digitalWrite(L_PWM, 0);
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
  else if(cmd == "led") {
    digitalWrite(ledPin, state);
  }
}
