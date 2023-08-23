#include "ECModule.h"
#include <Arduino.h>
#include "TempModule.h"
#include "DFRobot_ESP_EC.h"
#include "EEPROM.h"

ECModule::ECModule(int ecAdsPin, Adafruit_ADS1115 &ads, TempModule &tempModule)
    : _ecAdsPin(ecAdsPin), _ads(ads), _tempModule(tempModule)
{
}

void ECModule::begin()
{
    EEPROM.begin(32); // needed EEPROM.begin to store calibration k in eeprom
    _ec.begin();      // by default lib store calibration k since 10 change it by set ec.begin(30); to start from 30
}

float ECModule::readEc()
{
    if (millis() - _previousMillis > 400U)
    {
        // Serial.print("EC Buffer: ");
        // Serial.println(_analogBuffer[_bufferCounter]);
        float voltage = _ads.readADC_SingleEnded(_ecAdsPin) / 10;
        // Serial.print("EC Voltage: ");
        // Serial.println(voltage, 4);
        float temperatureC = _tempModule.readSensor();
        // Serial.print("EC Temperature: ");
        // Serial.println(temperatureC);
        float finalEc = _ec.readEC(voltage, temperatureC);
        _previousMillis = millis();
        // Serial.print("EC in func: ");
        // Serial.println(finalEc);
        return finalEc;
    }
    return 0;
}

bool ECModule::readSensor()
{
    float tmp_value = readEc();
    if (tmp_value != 0)
    {
        _ecValue = tmp_value;
        return true;
    }
    return false;
}

float ECModule::getValue()
{
    return _ecValue;
}