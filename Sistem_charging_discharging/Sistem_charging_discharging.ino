#define i_PV  A4
#define v_PV  A5
#define i_Bat A6
#define v_Bat A7
#define i_Load A8

int
PPWM              = 0,
PWM               = 0,
TOP               = 320,
pwmMaxLimited     = 0,
pwmMax            = 0,
avgCountCS = 100;

float dt, qtot, dsoc, soc = 68;
unsigned long timer;
  
float
PWM_MaxDC         = 97.0000,
voltagePV         = 0.0000,
voltageBAT        = 0.0000,
currentPV         = 0.0000,
currentBAT        = 0.0000,
currentLOAD       = 0.0000,
fltVolt           = 13.500,
chgVoltage        = 14.700,
fltCurrent        = 6.0000,
chgCurrent        = 12.000,
voltageThreshold  = 1.5000,
CS1               = 0.0000,
CS2               = 0.0000,
CS3               = 0.0000;

unsigned long lastTimeSerial = 0;
unsigned long timerDelaySerial = 1000; //1 detik

void setup() {
  Serial.begin(115200);
  TCCR5A  = 0;
  TCCR5B  = 0;
  TCNT5   = 0;
  TCCR5A = _BV(COM5C1) | _BV(WGM51);          
  TCCR5B = _BV(WGM53) | _BV(CS50);            
  ICR5=TOP;                                   

  pwmMax = TOP-1;
  pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000; 
  delay(500);
 
  pinMode(44, OUTPUT);       //PWM pin 44
  pinMode(47, OUTPUT);        //Relay charge
  pinMode(49, OUTPUT);       //Relay discharge
  digitalWrite(47, LOW);        
  digitalWrite(49, HIGH);        
}

void relay(float soc){
  if (soc >= 100) {
    digitalWrite(47, HIGH);      //cutoff charging
  }
  else if (soc <= 95) {
    digitalWrite(47, LOW);       //charging battery
  }
  if (soc >= 60) {
    digitalWrite(49, HIGH);     //discharging battery
  }
  else if (soc <= 20) {
    digitalWrite(49, LOW);      //cutoff discharging from battery (use PLN supply)
  } 
}

void predictivePWM(){                          
  if(voltagePV <= 0){PPWM = 0;}                                
  else{PPWM = (voltageBAT / voltagePV) * 100.00;}         
  PPWM = constrain(PPWM,0,pwmMaxLimited);
}

void PWM_Modulation(){
  predictivePWM();                                                       
  PWM = constrain(PWM,PPWM,pwmMaxLimited);
  OCR5C = PWM;
}

float readVoltage(int pinV) {
  int avgCountVS = 100;
  float VS, voltage;
  VS = 0.0000;     
   for(int i = 0; i<avgCountVS; i++) {
    VS = VS + ((analogRead(pinV) * 5.0) / 1024.0);
  }
  voltage = (VS/avgCountVS)*5.0;
  return voltage;
}

float readCurrentPV() {
  CS1 = 0.0000; 
  for(int i = 0; i<avgCountCS; i++) {
    CS1 = CS1 + ((analogRead(i_PV) * 5.0) / 1024.0);
  }
  currentPV  = (2.50 - (CS1/avgCountCS)) / 0.100;
  if (currentPV < 0) currentPV = 0;
  return currentPV;
}

float readCurrentBAT() {
  CS2 = 0.0000;     
  for(int i = 0; i<avgCountCS; i++) {
    CS2 = CS2 + ((analogRead(i_Bat) * 5.0) / 1024.0);
  }
  currentBAT  = (2.50 - (CS2/avgCountCS)) / 0.100;
  if (currentBAT < 0) currentBAT = 0;
  return currentBAT;
}

float readCurrentLOAD() {
  CS3 = 0.0000;     
  for(int i = 0; i<avgCountCS; i++) {
    CS3 = CS3 + ((analogRead(i_Load) * 5.0) / 1024.0);
  }
  currentLOAD  = (2.46 - (CS3/avgCountCS)) / 0.100;
  if (currentLOAD < 0) currentLOAD = 0;
  return currentLOAD;
}

float readSOC(float currentBAT, float currentLOAD, float dt){
  qtot = (currentBAT * dt) - (currentLOAD * dt);
  timer = millis();
  dsoc = qtot / 120 / 3600 * 100; //Battery capacity 120Ah
  soc = soc + dsoc;
  if (soc >= 100) {
      soc = 100;
  }
  return soc; 
}

void loop() {
  currentPV = readCurrentPV();
  currentBAT = readCurrentBAT();
  currentLOAD = readCurrentLOAD();
  
  voltagePV   = readVoltage(v_PV);
  voltageBAT  = readVoltage(v_Bat);
  dt = (millis() - timer) / 1000; //Time in second
  Serial.println(dt,6);
  soc = readSOC(currentBAT, currentLOAD, dt);
  timer = millis();
  relay(soc);

  if(voltagePV < voltageBAT + voltageThreshold) PWM = 0;
  if(voltageBAT < chgVoltage) PWM++; 
  if(voltageBAT > chgVoltage) PWM--;

  PWM_Modulation();
  
  if ((millis() - lastTimeSerial) > timerDelaySerial) {  
  Serial.print(voltagePV);
  Serial.print(", ");
  Serial.print(voltageBAT);
  Serial.print(", ");
  Serial.print(currentPV);
  Serial.print(", ");
  Serial.print(currentBAT);
  Serial.print(", ");
  Serial.print(currentLOAD);
  Serial.print(", ");
  Serial.print(soc, 2);
  Serial.print(", ");
  Serial.println(OCR5C);
  lastTimeSerial = millis();
  }
}
