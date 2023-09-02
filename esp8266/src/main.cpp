#include <Arduino.h>
#include "TDSModule.h"  // Class for TDS sensor
#include "PhModule.h"   // Class for pH sensor
#include "TempModule.h" // Class for temperature sensor
#include "MQTTModule.h" // Class for MQTT communication
#include "ECModule.h"   // Class for EC sensor
#include "DisplayModule.h"
#include "Adafruit_ADS1X15.h"
#include "wire.h"
#include "PCF8575.h"
#include <SPI.h>

#define WIFI_SSID "Lonsher-Office"
#define WIFI_PASSWORD "62306230"
#define MQTT_HOST IPAddress(192, 168, 1, 200)
#define MQTT_PORT 1883
#define MQTT_TOPIC "Hydro/esp"

const int ECAdsPin = 2;
const int TdsAdsPin = 0;
const int PhAdsPin = 1;
const int tempPin = 0;
bool tdsReady = false;
bool phReady = false;
unsigned long tdsPreviousMillis = 0;
unsigned long phPreviousMillis = 0;
unsigned long mqttPreviousMillis = 0;

PCF8575 pcf8575(0x20);
// Adafruit_ADS1115 ads1;
// TempModule tempModule(tempPin);
// PhModule phModule(PhAdsPin, ads1);
// TDSModule tdsModule(TdsAdsPin, ads1, tempModule);
// ECModule ecModule(ECAdsPin, ads1, tempModule);
// MQTTModule mqttModule(WIFI_SSID, WIFI_PASSWORD, MQTT_HOST, MQTT_PORT);
DisplayModule displayModule(pcf8575);

void setup()
{
  Serial.begin(9600);
  Serial.println("Start");
  for (int i = 0; i < 8; i++)
  {
    pcf8575.pinMode(i, OUTPUT);
  }

  Serial.println("after pinMode");
  pcf8575.begin();
  Serial.println("after begin");
  // tdsModule.begin();
  // phModule.begin();
  // tempModule.begin();
  // ecModule.begin();
  // mqttModule.begin();
  displayModule.begin();
  // ads1.setGain(GAIN_ONE);`
  // ads1.begin();
  Serial.println("Start");
}

void loop()
{
  // Serial.println("loop");
  // pcf8575.digitalWrite(P0, HIGH);
  // delay(1000);
  // Serial.println("after delay");
  // pcf8575.digitalWrite(P0, LOW);
  // delay(1000);
  // if (tdsModule.readSensor())
  // {
  //   Serial.printf("tds value %f, time %lu\n", tdsModule.getValue(), millis() - tdsPreviousMillis);
  //   tdsPreviousMillis = millis();
  // }
  // if (phModule.readSensor())
  // {
  //   Serial.printf("ph value %f, time %lu\n", phModule.getValue(), millis() - phPreviousMillis);
  //   phPreviousMillis = millis();
  // }
  // tdsModule.readSensor();
  // phModule.readSensor();
  // tempModule.readSensor();
  // ecModule.readSensor();
  // if (millis() - mqttPreviousMillis > 5000)
  // {
  //   mqttPreviousMillis = millis();
  //   // mqttModule.publishData(MQTT_TOPIC, tdsModule.getValue(), phModule.getValue(), tempModule.getValue(), ecModule.getValue());
  //   char influxDBLineProtocol[128]; // Adjust buffer size as per your requirements
  //   sprintf(influxDBLineProtocol, "esp0,hydro_number=0 tds_value=%f,ph_value=%f,temp_value=%f,ec_value=%f", tdsModule.getValue(), phModule.getValue(), tempModule.getValue(), ecModule.getValue());
  //   Serial.println(influxDBLineProtocol);
  // }
  displayModule.loop();
}