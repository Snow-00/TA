#ifndef BUCK_CONVERTER_H
#define BUCK_CONVERTER_H

#include "Arduino.h"

// converter sensors
#define v_PV  A0
#define i_PV  A1
#define v_Bat A2
#define i_Bat A3
#define i_Load A4

// converter pins
#define pwmPin 8
#define chargePin 9
#define dischargePin 10

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
