#include "BuckConverter.h"

int
PPWM              = 0, 
TOP               = 320,
pwmMaxLimited     = 0,
pwmMax            = 0,
avgCount = 100;

float qtot, dsoc, soc = 50;
  
float
PWM_MaxDC         = 97.0000,
chgVoltage        = 14.700,
voltageThreshold  = 1.5000;

void initBuck() {
  TCCR4A  = 0;
  TCCR4B  = 0;
  TCNT4   = 0;
  TCCR4A = _BV(COM4C1) | _BV(WGM41);          
  TCCR4B = _BV(WGM43) | _BV(CS40);            
  ICR4=TOP;                                   

  pwmMax = TOP-1;
  pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000;
  
  pinMode(pwmPin, OUTPUT); //PWM
  pinMode(chargePin, OUTPUT); //Relay charge//
  pinMode(dischargePin, OUTPUT); //Relay discharge
  
  digitalWrite(chargePin, LOW); //NC//
  digitalWrite(dischargePin, HIGH); //NC
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

void predictivePWM(float voltagePV, float voltageBAT){                          
  if(voltagePV <= 0){PPWM = 0;}                                
  else{PPWM = (voltageBAT / voltagePV) * 100.00;}         
  PPWM = constrain(PPWM,0,pwmMaxLimited);
}

int PWM_Modulation(float voltagePV, float voltageBAT, int PWM){
  predictivePWM(voltagePV, voltageBAT);                                                       
  PWM = constrain(PWM,PPWM,pwmMaxLimited);
  OCR4C = PWM;
  return PWM;
}

float readVoltagePV() {
  float VS, voltage;    
   for(int i = 0; i<avgCount; i++) {
    VS = VS + ((analogRead(v_PV) * 5.0) / 1024.0);
  }
  voltage = (VS/avgCount)*5.0;
  return voltage;
}

float readVoltageBat() {
  float VS, voltage;    
   for(int i = 0; i<avgCount; i++) {
    VS = VS + ((analogRead(v_Bat) * 5.0) / 1024.0);
  }
  voltage = (VS/avgCount)*5.0;
  return voltage;
}

float readCurrentPV() {
  float CS, current; 
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_PV) * 5.0) / 1024.0);
  }
  current  = (2.50 - (CS/avgCount)) / 0.100;
  if (current < 0) current = 0;
  return current;
}

float readCurrentBAT() {
  float CS, current;     
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_Bat) * 5.0) / 1024.0);
  }
  current  = (2.50 - (CS/avgCount)) / 0.100;
  if (current < 0) current = 0;
  return current;
}

float readCurrentLOAD() {
  float CS, current;     
  for(int i = 0; i<avgCount; i++) {
    CS = CS + ((analogRead(i_Load) * 5.0) / 1024.0);
  }
  current  = (2.46 - (CS/avgCount)) / 0.100;
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
