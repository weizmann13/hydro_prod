#ifndef ECMODULE_H
#define ECMODULE_H

#include <Arduino.h>
#include "TempModule.h"
#include "Adafruit_ADS1X15.h"
#include "DFRobot_ESP_EC.h"
#include "EEPROM.h"

class ECModule
{
public:
    ECModule(int ecAdsPin, Adafruit_ADS1115 &ads, TempModule &tempModule);

    void begin();
    bool readSensor();
    float getValue();

private:
    float readEc();

    DFRobot_ESP_EC _ec;
    int _ecAdsPin;
    Adafruit_ADS1115 &_ads;
    TempModule &_tempModule;
    unsigned long _previousMillis = 0;
    int _tempPin;
    float _ecValue = 0;
};

#endif