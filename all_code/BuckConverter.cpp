#include "BuckConverter.h"

float
SOC = 20.00,
VS,
voltage,
CS,
current;

const int 
avgCountVS = 10,
avgCountCS = 10;

void initBuck() {
  TCCR4A  = 0;
  TCCR4B  = 0;
  TCNT4   = 0;
  // Setting Timer 4 Clear on compare match dan Phase Correct PWM 
  TCCR4A = _BV(COM4C1) | _BV(WGM41);
  TCCR4B = _BV(WGM43) | _BV(CS40);        // No prescaler (N)
  ICR4=320;                               // PWM ferq. = F_CPU/(2*N*ICRn)
  OCR4C = 20;
  
  pinMode(pwmPin, OUTPUT); //PWM
  pinMode(chargePin, OUTPUT); //Relay charge//
  pinMode(dischargePin, OUTPUT); //Relay discharge
  
  digitalWrite(chargePin, HIGH); //NC//
  digitalWrite(dischargePin, LOW); //NC
}

float readVoltage(int pinV) {
  VS = 0.0000;      // bersihkan value di VS
   for(int i = 0; i<avgCountVS; i++) {
    VS = VS + ((analogRead(pinV) * 5.0) / 1024.0);
  }
  voltage = (VS/avgCountVS) * 5.0;
  return voltage;
}

float readCurrent(int pinI) {
  CS = 0.0000;      // bersihkan value di CS
  for(int i = 0; i<avgCountCS; i++) {
    CS = CS + analogRead(pinI);
  }
  current = CS / avgCountCS;
  current = (((current * 5.0) / 1024.0) - 2.5) / 0.100; 

  if (current < 0) {
    current = 0;
  }
  return current;
}

float readSOC(float currentBAT, float currentLOAD){
  int dt, qtot, dsoc;
  unsigned long timer;
  
  dt = (millis() - timer) / 1000; //Time in second
  qtot = (currentBAT * dt) - (currentLOAD * dt);
  timer = millis();
  dsoc = qtot * 100 / 120 / 3600; //Battery capacity 120Ah
  SOC = SOC + dsoc;
  return SOC;
}

void relay(float SOC){
  if (SOC >= 100) {
    digitalWrite(chargePin, LOW); //cutoff charging
  }
  else if (SOC <= 95) {
    digitalWrite(chargePin, HIGH); //charging battery
  }
  if (SOC >= 60) {
    digitalWrite(dischargePin, HIGH); //discharging battery
  }
  else if (SOC <= 20) {
    digitalWrite(dischargePin, LOW); //cutoff discharging from battery (use PLN supply)
  } 
}
