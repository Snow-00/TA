#ifndef BUCK_CONVERTER_H
#define BUCK_CONVERTER_H

#include "Arduino.h"

// converter pins
#define pwmPin 8
#define chargePin 9
#define dischargePin 10

void initBuck();
float readVoltage(int pinV);
float readCurrent(int pinI);
float readSOC(float currentBAT, float currentLOAD);
void relay(float SOC);

#endif
