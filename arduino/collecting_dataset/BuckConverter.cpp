#include "BuckConverter.h"

int avgCount = 100;

float qtot, dsoc, soc = 100;

void initBuck() {
  pinMode(chargePin, OUTPUT); //Relay charge//
  pinMode(dischargePin, OUTPUT); //Relay discharge
  
  digitalWrite(chargePin, LOW); // low = connected
  digitalWrite(dischargePin, HIGH); // high = batt
}

void relay(float soc){
  if (soc >= 100) {
    digitalWrite(chargePin, HIGH);      //cutoff charging
  }
  else if (soc <= 95) {
    digitalWrite(chargePin, LOW);       //charging battery
  }
  if (soc >= 60) {
    digitalWrite(dischargePin, HIGH);     //discharging battery
  }
  else if (soc <= 20) {
    digitalWrite(dischargePin, LOW);      //cutoff discharging from battery (use PLN supply)
  } 
}

float readVoltageBat() {
  float VS, voltage; 
  VS=0;   
   for(int i = 0; i<avgCount; i++) {
    VS = VS + ((analogRead(v_Bat) * 5.0) / 1024.0);
  }
  voltage = (VS/avgCount)*5.0;
  return voltage;
}

float readCurrentLOAD() {
  float CS, current;     
  CS=0;
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_Load) * 5.0) / 1024.0);
  }
  current  = (2.5 - (CS/avgCount)) / 0.100;
  if (current < 0) current = 0;
  return current;
}

float readSOC(float currentLOAD, float dt){
  qtot = (currentLOAD * dt);
  dsoc = qtot / 120 / 3600 * 100; //Battery capacity 120Ah
  soc = soc - dsoc;
  if (soc >= 100) {
      soc = 100;
  }
  return soc; 
}
