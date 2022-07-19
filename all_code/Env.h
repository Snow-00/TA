#ifndef ENV_H
#define ENV_H

#include "Arduino.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "DHT.h"
#include "RBDdimmer.h"

// env sensors pins
#define ONE_WIRE_BUS 30

#define DHTPIN A0
#define DHTPIN2 A1
#define DHTPIN3 A2
#define DHTPIN4 A3
#define DHTTYPE DHT22

// env actuator pin
#define R_PWM 4
#define L_PWM 5
#define R_EN 1
#define L_EN 7
#define pinPump 3
#define ledPin  2

void initEnv();
float readHumid();
float readTemp();
void driveAct(String cmd, int state);

#endif
