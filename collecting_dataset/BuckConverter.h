#ifndef BUCK_CONVERTER_H
#define BUCK_CONVERTER_H

#include "Arduino.h"

// converter sensors
#define i_Bat A6
#define v_Bat A7
#define i_Load A8

// converter pins
#define chargePin 47
#define dischargePin 49

void initBuck();
float readVoltageBat();
float readCurrentLOAD();
float readSOC(float currentLOAD, float dt);
void relay(float soc);

#endif
