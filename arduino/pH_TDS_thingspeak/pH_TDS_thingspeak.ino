#include <WiFi.h>
#include <WiFiMulti.h>

#include "DFRobot_PH.h"
#include "GravityTDS.h"
#include <EEPROM.h>

#define phPin 12          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin 13

DFRobot_PH ph;
GravityTDS gravityTds;
WiFiMulti WiFiMulti;

const char* ssid     = "OPPO"; // Your SSID (Name of your WiFi)
const char* password = "12345678"; //Your Wifi password

const char* host = "api.thingspeak.com";
String api_key = "GPFMAB99YI1AZDY9"; // Your API Key provied by thingspeak
float voltage, phValue, tdsValue, temperature = 25;

void setup() {
  Serial.begin(9600);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  ph.begin();

  Connect_to_Wifi();
}

void loop() {

  phValue = readPh();
  tdsValue = readTds();

  if (isnan(phValue) || isnan(tdsValue)) {
    Serial.println(F("Failed to read from pH or TDS sensor!"));
    return;
  }

  Serial.print("pH: ");
  Serial.println(phValue, 2);
  Serial.print("Tds: ");
  Serial.println(tdsValue, 2);

  // call function to send data to Thingspeak
  Send_Data(phValue, tdsValue);

  delay(100);
}

void Connect_to_Wifi()
{

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

float readPh() {
  voltage = analogRead(phPin) / 1024.0 * 5000; // read the voltage
  phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
  return phValue;
}

float readTds() {
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  //  tdsValue = tdsValue / 500;
  return tdsValue;
}

void Send_Data(float t, float h)
{

  // map the moist to 0 and 100% for a nice overview in thingspeak.
  Serial.println("");
  Serial.println("Prepare to send data");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  else
  {
    String data_to_send = api_key;
    data_to_send += "&field1=";
    data_to_send += String(h);
    data_to_send += "&field2=";
    data_to_send += String(t);
    data_to_send += "&field3=";
    data_to_send += String("test");

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data_to_send.length());
    client.print("\n\n");
    client.print(data_to_send);

    delay(1000);
  }

  client.stop();

}
