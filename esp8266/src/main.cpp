#include <Arduino.h>
#include "TDSModule.h"  // Class for TDS sensor
#include "PhModule.h"   // Class for pH sensor
#include "TempModule.h" // Class for temperature sensor
#include "MQTTModule.h" // Class for MQTT communication
#include "DisplayModule.h"

#define WIFI_SSID "Lonsher-Office"
#define WIFI_PASSWORD "62306230"
#define MQTT_HOST IPAddress(192, 168, 1, 200)
#define MQTT_PORT 1883
#define MQTT_TOPIC "Hydro/esp"

const int TdsPhPin = A0;
const int TdsPowerPin = 4;
const int PhPowerPin = 5;
const int tempPin = 0;
bool tdsReady = false;
bool phReady = false;
unsigned long tdsPreviousMillis = 0;
unsigned long phPreviousMillis = 0;

TDSModule tdsModule(TdsPhPin, TdsPowerPin, tempPin);
PhModule phModule(TdsPhPin, PhPowerPin);
TempModule tempModule(tempPin);
MQTTModule mqttModule(WIFI_SSID, WIFI_PASSWORD, MQTT_HOST, MQTT_PORT);
DisplayModule displayModule(tdsModule, phModule, tempModule);

void setup()
{
  Serial.begin(9600);
  tdsModule.begin();
  phModule.begin();
  tempModule.begin();
  mqttModule.begin();
  displayModule.begin();
}

void loop()
{

  if (tdsModule.readSensor())
  {
    Serial.printf("tds value%f, time %lu\n", tdsModule.getValue(), millis() - tdsPreviousMillis);
    tdsPreviousMillis = millis();
  }
  if (phModule.readSensor())
  {
    Serial.printf("ph value%f, time %lu\n", phModule.getValue(), millis() - phPreviousMillis);
    phPreviousMillis = millis();
  }
  tempModule.readSensor();

  // mqttModule.publishData(MQTT_TOPIC, tdsModule.getValue(), phModule.getValue(), tempModule.getValue());
  displayModule.loop();
  // delay(1000);
}