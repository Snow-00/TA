#include "BuckConverter.h"

int
PPWM              = 0, 
TOP               = 320,
pwmMaxLimited     = 0,
pwmMax            = 0,
avgCount = 100;

float qtot, dsoc, soc = 93;
  
float
PWM_MaxDC         = 97.0000,
chgVoltage        = 14.400,
voltageThreshold  = 1.5000;

void initBuck() {
  TCCR5A  = 0;
  TCCR5B  = 0;
  TCNT5   = 0;
  TCCR5A = _BV(COM5C1) | _BV(WGM51);          
  TCCR5B = _BV(WGM53) | _BV(CS50);            
  ICR4=TOP;                                   

  pwmMax = TOP-1;
  pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000;
  
  pinMode(pwmPin, OUTPUT); //PWM
  pinMode(chargePin, OUTPUT); //Relay charge//
  pinMode(dischargePin, OUTPUT); //Relay discharge
  
  digitalWrite(chargePin, HIGH); // high = connected
  digitalWrite(dischargePin, LOW); // low = batt
}

void relay(float soc){
  if (soc >= 100) {
    digitalWrite(chargePin, LOW);      //cutoff charging
  }
  else if (soc <= 95) {
    digitalWrite(chargePin, HIGH);       //charging battery
  }
  if (soc >= 60) {
    digitalWrite(dischargePin, LOW);     //discharging battery
  }
  else if (soc <= 20) {
    digitalWrite(dischargePin, HIGH);      //cutoff discharging from battery (use PLN supply)
  } 
}

void predictivePWM(float voltagePV, float voltageBAT){                          
  if(voltagePV <= 0){PPWM = 0;}                                
  else{PPWM = (voltageBAT / voltagePV) * 100.00;}         
  PPWM = constrain(PPWM,0,pwmMaxLimited);
}

int PWM_Modulation(float voltagePV, float voltageBAT, int PWM){
  predictivePWM(voltagePV, voltageBAT);                                                       
  PWM = constrain(PWM,PPWM,pwmMaxLimited);
  OCR5C = PWM;
  return PWM;
}

float readVoltagePV() {
  float VS, voltage; 
  VS=0;   
   for(int i = 0; i<avgCount; i++) {
    VS = VS + ((analogRead(v_PV) * 5.0) / 1024.0);
  }
  voltage = (VS/avgCount)*5.0;
  return voltage;
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

float readCurrentPV() {
  float CS, current; 
  CS=0;
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_PV) * 5.0) / 1024.0);
  }
  current  = (2.473 - (CS/avgCount)) / 0.100;
  if (current < 0) current = 0;
  return current * 100;
}

float readCurrentBAT() {
  float CS, current;     
  CS=0;
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_Bat) * 5.0) / 1024.0);
  }
  current  = (2.455 - (CS/avgCount)) / 0.100;
  if (current < 0) current = 0;
  return current;
}

float readCurrentLOAD() {
  float CS, current;     
  CS=0;
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_Load) * 5.0) / 1024.0);
  }
  current  = (2.42 - (CS/avgCount)) / 0.100;
  if (current < 0) current = 0;
  return current;
}

float readSOC(float currentBAT, float currentLOAD, float dt){
  qtot = (currentBAT * dt) - (currentLOAD * dt);
  dsoc = qtot / 120 / 3600 * 100; //Battery capacity 120Ah
  soc = soc + dsoc;
  if (soc >= 100) {
      soc = 100;
  }
  return soc; 
}
