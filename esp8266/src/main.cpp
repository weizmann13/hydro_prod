#include <Arduino.h>
#include "TDSModule.h"  // Class for TDS sensor
#include "PhModule.h"   // Class for pH sensor
#include "TempModule.h" // Class for temperature sensor
#include "MQTTModule.h" // Class for MQTT communication

#define WIFI_SSID "Lonsher-Office"
#define WIFI_PASSWORD "62306230"
#define MQTT_HOST IPAddress(192, 168, 1, 200)
#define MQTT_PORT 1883
#define MQTT_TOPIC "Hydro/esp"

TDSModule tdsModule;
PhModule phModule;
TempModule tempModule;
MQTTModule mqttModule(WIFI_SSID, WIFI_PASSWORD, MQTT_HOST, MQTT_PORT);

void setup()
{
  Serial.begin(9600);
  tdsModule.begin();
  phModule.begin();
  tempModule.begin();
  mqttModule.begin()
}

void loop()
{
  tdsModule.readSensor();
  phModule.readSensor();
  tempModule.readSensor();

  mqttModule.publishData(MQTT_TOPIC, tdsModule.getValue(), phModule.getValue(), tempModule.getValue());

  delay(1000);
}
