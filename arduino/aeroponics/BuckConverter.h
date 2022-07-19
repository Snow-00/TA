#ifndef BUCK_CONVERTER_H
#define BUCK_CONVERTER_H

#include "Arduino.h"

// converter sensors
#define i_PV  A4
#define v_PV  A5
#define i_Bat A6
#define v_Bat A7
#define i_Load A8

// converter pins
#define pwmPin 44
#define chargePin 47
#define dischargePin 49

void initBuck();
void predictivePWM(float voltagePV, float voltageBAT);
int PWM_Modulation(float voltagePV, float voltageBAT, int PWM);
float readVoltagePV();
float readVoltageBat();
float readCurrentPV();
float readCurrentBAT();
float readCurrentLOAD();
float readSOC(float currentBAT, float currentLOAD, float dt);
void relay(float soc);

#endif
