#ifndef ENV_H
#define ENV_H

#include "Arduino.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "DHT.h"
#include "RBDdimmer.h"

// env sensors pins
#define ONE_WIRE_BUS 3

#define DHTPIN A0
#define DHTPIN2 A1
#define DHTPIN3 A2
#define DHTPIN4 A3
#define DHTTYPE DHT22

// env actuator pin
#define R_PWM 10
#define L_PWM 11
#define R_EN 8
#define L_EN 9
#define pinPump 7
#define ledPin  4

void initEnv();
float readHumid();
float readTemp();
void driveAct(String cmd, int state);

#endif
