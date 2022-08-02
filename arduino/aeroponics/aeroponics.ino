#include "NutrientMix.h"
#include "Env.h"
#include "BuckConverter.h"
#include <EEPROM.h>
#define i_PV  A4
#define v_PV  A5
#define i_BAT A6
#define v_BAT A7
#define i_LOAD A8
#define pwmPin 44
#define chargePin 47
#define dischargePin 49
int
PPWM              = 0,
PWM               = 0,
TOP               = 320,
pwmMaxLimited     = 0,
pwmMax            = 0,
avgCountCS        = 100,
avgCountVS        = 10,
modeChg           = 0,
modeDcg           = 0,
input             = 0;
float dt, qtot, dsoc, soc = 100;
float 
levelVal, 
humidVal,
tempVal,
float
PWM_MaxDC         = 97.0000,
voltagePV         = 0.0000,
voltageBAT        = 0.0000,
currentPV         = 0.0000,
currentBAT        = 0.0000,
currentLOAD       = 0.0000,
fltVolt           = 13.500,
chgVoltage        = 14.400,
fltCurrent        = 6.0000,
chgCurrent        = 12.000,
voltageThreshold  = 1.5000,
CS1               = 0.0000,
CS2               = 0.0000,
CS3               = 0.0000,
VS1               = 0.0000,
VS2               = 0.0000;


int 
indexComma, 
len, 
cond;

String 
command, 
arrCmd[3],
readInput;

unsigned long timer, timerBat;
//unsigned long lastTimeSerial = 0;
//unsigned long timerDelaySerial = 1000; //1 detik

void setup()
{
  Serial.begin(115200);
  initNutrient();
  initEnv();
  TCCR5A  = 0;
  TCCR5B  = 0;
  TCNT5   = 0;
  // Setting Timer 4 Clear on compare match dan Phase Correct PWM
  TCCR5A = _BV(COM5C1) | _BV(WGM51);          //
  TCCR5B = _BV(WGM53) | _BV(CS50);            // No prescaler (N)
  ICR5 = TOP;                                 // PWM ferq. = F_CPU/(2*N*ICRn)

  pwmMax = TOP - 1;
  pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000; //Mencari Duty Cycle PWM maksimum (proteksi pembatasan pwm)
  delay(500);
  pinMode(pwmPin, OUTPUT);
  pinMode(chargePin, OUTPUT);                         //Relay charge
  pinMode(dischargePin, OUTPUT);                        //Relay discharge
  digitalWrite (chargePin, LOW);                       //Relay charge OFF
  digitalWrite (dischargePin, HIGH);  
  timer = millis();
  timerBat = millis();
}
void relay(float soc) {
  if (soc >= 100) {
    modeChg = 1;
    digitalWrite(47, HIGH);      //cutoff charging
  }
  else if (soc <= 95) {
    modeChg = 2;
    digitalWrite(47, LOW);       //charging battery
  }
  if (soc >= 60) {
    modeDcg = 1;
    digitalWrite(49, HIGH);     //discharging battery
  }
  else if (soc <= 20) {
    modeDcg = 2;
    digitalWrite(49, LOW);      //cutoff discharging from battery (use PLN supply)
  }
}
void predictivePWM() {                               //Agoritma PWM prediktif
  if (voltagePV <= 0) {
    PPWM = 0; //Cegah out PWM tidak terbatas ketika voltasePV = nol
  }
  else {
    PPWM = (voltageBAT / voltagePV) * 100.00; //Hitung untuk PWM prediktif dan simpan dalam variabel PWM
  }
  PPWM = constrain(PPWM, 0, pwmMaxLimited);
}
void PWM_Modulation() {
  predictivePWM();                                    //Jalankan dan hitung untuk pwm prediktif
  PWM = constrain(PWM, PPWM, pwmMaxLimited);
  OCR5C = PWM;
}
float readVoltage_PV() {
  VS1 = 0.0000;      // bersihkan value di VS
  for (int i = 0; i < avgCountVS; i++) {
    VS1 = VS1 + ((analogRead(v_PV) * 5.0) / 1024.0);
  }
  voltagePV = (VS1 / avgCountVS) * 5.0;
  return voltagePV;
}

float readVoltage_BAT() {
  VS2 = 0.0000;      // bersihkan value di VS
  for (int i = 0; i < avgCountVS; i++) {
    VS2 = VS2 + ((analogRead(v_BAT) * 5.0) / 1024.0);
  }
  voltageBAT = (VS2 / avgCountVS) * 5.0;
  return voltageBAT;
}
float readCurrentPV() {
  CS1 = 0.0000;     // bersihkan value di CS1
  for (int i = 0; i < avgCountCS; i++) {
    CS1 = CS1 + ((analogRead(i_PV) * 5.0) / 1024.0);
  }
  currentPV  = (2.67 - (CS1 / avgCountCS)) / 0.100;

  if (currentPV < 0) currentPV = 0;
  return currentPV;
}
float readCurrentBAT() {
  CS2 = 0.0000;     // bersihkan value di CS1
  for (int i = 0; i < avgCountCS; i++) {
    CS2 = CS2 + ((analogRead(i_BAT) * 5.0) / 1024.0);
  }
  currentBAT  = (2.35 - (CS2 / avgCountCS)) / 0.100;

  if (currentBAT < 0) currentBAT = 0;
  return currentBAT;
}

float readCurrentLOAD() {
  CS3 = 0.0000;     // bersihkan value di CS1
  for (int i = 0; i < avgCountCS; i++) {
    CS3 = CS3 + ((analogRead(i_LOAD) * 5.0) / 1024.0);
  }
  currentLOAD  = (2. - (CS3 / avgCountCS)) / 0.100;

  if (currentLOAD < 0) currentLOAD = 0;
  return currentLOAD;
}

float readSOC(float currentBAT, float currentLOAD, float dt) {
  //  qtot = (currentBAT * dt) - (currentLOAD * dt);
  qtot = (currentBAT * dt);
  dsoc = qtot / 120 / 3600 * 100; //Battery capacity 120Ah
  soc = soc + dsoc;
  if (soc >= 100) {
    soc = 100;
  }
  return soc;
}


void getInput() {
  int countCmd = 0;
  readInput = Serial.readString();
  readInput.trim();

  // Split the string into substrings
  while (true)
  {
    int index = readInput.indexOf(';');
    if (index == -1) // No space found
    {
      arrCmd[countCmd++] = readInput;
      break;
    }
    arrCmd[countCmd++] = readInput.substring(0, index);
    readInput = readInput.substring(index + 1);
  }

  for (int i = 0; i < countCmd; i++)
  {
    indexComma = arrCmd[i].lastIndexOf(',');
    len = arrCmd[i].length();
    command = arrCmd[i].substring(0, indexComma);
    cond = arrCmd[i].substring(indexComma + 1, len).toInt();

    pump(command, cond);
    driveAct(command, cond);
  }
}

void loop()
{
  if (Serial.available() > 1) {
    getInput();
  }

  currentPV = readCurrentPV();
  currentBAT = readCurrentBAT();
  currentLOAD = readCurrentLOAD();

  voltagePV   = readVoltage_PV();
  voltageBAT  = readVoltage_BAT();
  dt = millis() - timerBat;
  dt = dt / 1000; //Time in secondsecond
  Serial.println(dt,6);
  socBat = readSOC(currentBAT, currentLOAD, dt);
  timerBat = millis();
  relay(socBat);

  if (voltagePV < voltageBAT + voltageThreshold) PWM = 0;
  if (voltageBAT < chgVoltage) PWM++;
  if (voltageBAT > chgVoltage) PWM--;

  PWM_Modulation();
  
  if (millis() - timer > 1000) {
    timer = millis ();
    levelVal = readLevel();

    humidVal = readHumid();
    tempVal = readTemp();
    
    Serial.print(humidVal, 2);
    Serial.print(",");
    Serial.print(tempVal, 2);
    Serial.print(",");
    Serial.print(levelVal, 2);
    Serial.print(",");
    Serial.print(voltagePV, 2);
    Serial.print(", ");
    Serial.print(voltageBAT, 2);
    Serial.print(", ");
    Serial.println(socBat,5);
  }
}
