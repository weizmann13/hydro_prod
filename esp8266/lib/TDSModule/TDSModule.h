#ifndef TDSMODULE_H
#define TDSMODULE_H

#include <Arduino.h>
#include "TempModule.h"
#include "Adafruit_ADS1X15.h"

class TDSModule
{
public:
    TDSModule(int tdsAdsPin, Adafruit_ADS1115 &ads, TempModule &tempModule);
    TDSModule(int tdsAdsPin, Adafruit_ADS1115 &ads, TempModule &tempModule, int SCOUNT);

    void begin();
    bool readSensor();
    float getValue();

private:
    float getAverageNum(int bArray[], int iFilterLen);
    float readTds();

    int _tdsAdsPin;
    Adafruit_ADS1115 &_ads;
    TempModule &_tempModule;
    int _SCOUNT = 10;
    unsigned long _previousMillis = 0;
    int *_analogBuffer;
    int _bufferCounter = 0;
    int _tdsValue = 0;
};

#endif